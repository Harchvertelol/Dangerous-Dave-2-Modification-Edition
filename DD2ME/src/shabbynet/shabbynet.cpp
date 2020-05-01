/*
ShabbyNet. Copyright (c) 2013 by Andrew Stroganov <savhte@gmail.com>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
#include <string>
#include <cassert>
#include <ctime>
#include <stdexcept>

#include <windows.h>
#include <winsock2.h>

#include "shabbynet.hpp"

using std::cout;
using std::endl;

/////////////////////////STATICNET_CPP///////////////////////////
namespace net {

shabby_net::Network& inst()
{
	static shabby_net::Network n;
	return n;
}

void run(int ms)
{
	inst().run(ms);
}

void listen(int port)
{
	inst().listen(port);
}

void accept(shabby_net::Socket* s)
{
	inst().accept(s);
}

} // namespace net

namespace shabby_net {

typedef unsigned long long int uint64;


//////////////////////////NETWORK_CPP////////////////////////////
uint64 now_ms()
{
	SYSTEMTIME t;
	GetSystemTime(&t);
	return uint64(time(0)) * 1000 + uint64(t.wMilliseconds);
}

Network::~Network()
{
	if(listen_sock_ != -1) closesocket(listen_sock_);
	WSACleanup();
}

Network::Network(): listen_sock_(invalid_socket), acceptor_(0)
{
    WSADATA wsaData;
    WORD version;
    int error;
    version = MAKEWORD( 2, 0 );
    error = WSAStartup( version, &wsaData );
}

void Network::accept(Socket* s)
{
	if(acceptor_) throw std::logic_error("Already accepting");
	if(listen_sock_ == invalid_socket) throw std::logic_error("Accept failed. Call listen() first");

	acceptor_ = s;
}

void Network::listen(int port)
{
    listen_sock_ = socket( AF_INET, SOCK_STREAM, 0 );

    struct sockaddr_in sin;
    memset( &sin, 0, sizeof sin );
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons( port );

	BOOL reuse = TRUE;
	int reuse_len = sizeof(BOOL);

	if (setsockopt(listen_sock_, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, reuse_len) < 0)
		throw NetworkError("reuse failed", WSAGetLastError());

    assert ( bind( listen_sock_, (sockaddr*)&sin, sizeof sin ) != SOCKET_ERROR );
    ::listen( listen_sock_, SOMAXCONN );
}

bool Network::run_once(int timeout_ms)
{
	timeval timeout;
	timeout.tv_sec = timeout_ms / 1000;
	timeout.tv_usec = (timeout_ms % 1000) * 1000;

    fd_set fset_read;
    fd_set fset_write;
	FD_ZERO(&fset_read);
	FD_ZERO(&fset_write);

	if(acceptor_)
		FD_SET(listen_sock_, &fset_read);

	int nfd = 0;
	for(Sockets::iterator i = sockets_.begin(); i != sockets_.end(); ++i)
	{
		const Socket* s = *i;
		if(s->flags() & SF_READ)
		{
			FD_SET(s->native(), &fset_read);
			++nfd;
		}
		if(s->flags() & SF_WRITE)
		{
			FD_SET(s->native(), &fset_write);
			++nfd;
		}
	}

	if(nfd == 0 && !acceptor_) return false;

	const int result = select(0, &fset_read, &fset_write, 0, &timeout);

	if(result == SOCKET_ERROR)
		throw NetworkError("select() failed" , WSAGetLastError());
    else if(result != 0)
    {
		if(acceptor_ && FD_ISSET(listen_sock_, &fset_read))
			do_accept();

		Sockets tmp(sockets_);
		for(Sockets::iterator i = tmp.begin(); i != tmp.end(); ++i)
		{
			Socket* s = *i;

   			if(FD_ISSET(s->native(), &fset_read)) s->handle_read();
   			if(FD_ISSET(s->native(), &fset_write)) s->handle_write();
		}
	}
	return true;
}

void Network::run(int timeout_ms)
{
	while(timeout_ms)
	{
		const uint64 started = now_ms();
		if(!run_once(timeout_ms)) break;
		const uint64 now = now_ms();
		if(started > now) break;
		timeout_ms -= std::min(timeout_ms, int(now - started));
	}
}

void Network::do_accept()
{
	Socket* s = acceptor_;
	acceptor_ = 0;
	s->accept(listen_sock_);
//	sockets_.insert(s);
}

//////////////////////////SOCKET_CPP////////////////////////////

Socket::Socket(): sock_(-1), flags_(SF_NULL)
{
	net::inst().attach(this);
}

Socket::~Socket()
{
	net::inst().detach(this);
}

void Socket::handle_close()
{
	flags_ = SF_NULL;
	sock_ = invalid_socket;
	on_closed();
}

void Socket::send(const std::string& s)
{
	send(s.c_str(), s.size());
}

void Socket::send(const char* buf, int len)
{
	const int mtu = 512;

	while(len)
	{
		const int n = std::min(mtu, len);
		Buffer v(buf, buf+n);
		send_queue_.push(v);
		buf += n;
		len -= n;
	}

	set_flag(SF_WRITE);
}

void Socket::handle_write()
{
	if(send_queue_.empty())
	{
		unset_flag(SF_WRITE);
		on_sent();
		return;
	}

	Buffer& v = send_queue_.front();
	const int n = ::send(sock_, &v[0], v.size(), 0);
	send_queue_.pop();
	if(n <= 0) handle_close();
}

void Socket::handle_read()
{
	const int buf_size = 4096;
	char buf[buf_size];
    const int n = recv(sock_, buf, buf_size, 0);
	unset_flag(SF_READ);

	if(n <= 0) handle_close();
	else on_received(buf, n);
}

void Socket::read_some()
{
	set_flag(SF_READ);
}

void Socket::accept(socket_t s)
{
    int length;
    struct sockaddr_in sin;
    memset( &sin, 0, sizeof sin );
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons( 21 );
    length = sizeof sin;

    char *connected_ip = inet_ntoa(sin.sin_addr);
    int port = ntohs(sin.sin_port);
    cout << "New client connected: " << connected_ip<<":"<<port << endl;

	sock_ = ::accept(s, (sockaddr*)&sin, &length );
	if(sock_ == INVALID_SOCKET)
		throw NetworkError("accept() failed", WSAGetLastError());
	on_connected();
}

void Socket::set_flag(socket_flag_t f)
{
	flags_ |= f;
}

void Socket::unset_flag(socket_flag_t f)
{
	flags_ &= ~f;
}

void Socket::connect(const std::string& host, int port)
{
    LPHOSTENT hostEntry;
    hostEntry = gethostbyname(host.c_str());	// Specifying the server by its name;

    if (!hostEntry)
		throw NetworkError("Host name resolve failed", WSAGetLastError());

    sock_ = socket( AF_INET, SOCK_STREAM, 0 );

    SOCKADDR_IN serverInfo;
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
    serverInfo.sin_port = htons(port);		// Change to network-byte order and
    // insert into port field
    int nret = ::connect(sock_, (LPSOCKADDR)&serverInfo, sizeof(struct sockaddr));
    if (nret == SOCKET_ERROR)
		throw NetworkError("Connect failed", WSAGetLastError());
}



} // namespace shabby_net
