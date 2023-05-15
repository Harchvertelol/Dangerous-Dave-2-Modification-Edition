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

#ifndef SHABBYNET_HPP
#define SHABBYNET_HPP

#include <stdexcept>
#include <vector>
#include <queue>
#include <set>

namespace shabby_net {

class NetworkError: public std::runtime_error
{
public:
	NetworkError(const std::string& msg, int err): std::runtime_error(msg), err_(err) {}
	int error_code() const { return err_; }

private:
	int err_;
};

enum socket_flag_t
{
	SF_NULL = 0, 
	SF_READ = 1, 
	SF_WRITE = 1 << 1,
};

typedef int socket_t;
enum {invalid_socket = -1};

class Network;

class Socket
{
public:
	Socket();
	virtual ~Socket();

	int native() const { return sock_; }
	virtual void connect(const std::string& host, int port);
	void send(const char*, int);
	void send(const std::string&);
	void read_some();
	bool closed() const { return sock_ == invalid_socket; }

protected:
	virtual void on_received(const char*, int len) {}
	virtual void on_closed() {}
	virtual void on_connected() {}
	virtual void on_sent() {}

private:
	friend class Network;
	typedef std::vector<char> Buffer;

	void accept(socket_t);
	int flags() const { return flags_; }
	void set_flag(socket_flag_t);
	void unset_flag(socket_flag_t);
	void set_non_blocking();
	void handle_close();
	void handle_read();
	void handle_write();

	int sock_;
	int flags_;
	std::queue<Buffer> send_queue_;
};

class Network
{
public:
	Network();
	~Network();
	void run(int ms);
	void listen(int port);
	void accept(Socket*);
	void attach(Socket* s) { sockets_.insert(s); }
	void detach(Socket* s) { sockets_.erase(s); }

private:
	typedef std::set<Socket*> Sockets;
	void do_accept();
	bool run_once(int ms);

	Sockets sockets_;
	int listen_sock_;
	Socket* acceptor_;
};

} // namespace shabby_net

namespace net
{
	shabby_net::Network& inst();
	void run(int ms);
	void listen(int port);
	void accept(shabby_net::Socket*);
} // namespace net

using shabby_net::Socket;

#endif // SHABBYNET_HPP


