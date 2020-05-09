#include <iostream>

#include "SClient.h"
#include "Server.h"
#include "../UserData.h"

using namespace std;

SClient::SClient(Server* s):
    s_ID(0),
    s_Server(s),
    s_ReceiveBuffer("")
{
    s_UserData = new UserData;
}

SClient::~SClient()
{
    if(s_UserData != 0) delete s_UserData;
}

void SClient::on_received(const char* buf, int len)
{
    if(len < 0)
    {
        cout<<"Error! Length < 0."<<endl;
        read_some();
        return;
    }
    string str;
    s_ReceiveBuffer.append(buf, len);
    for(;;) // вдруг у нас несколько строк
    {
        const size_t pos = s_ReceiveBuffer.find('\n');
        if(pos != string::npos) // есть строка
        {
            str = s_ReceiveBuffer.substr(0, pos);
            s_ReceiveBuffer.erase(0, pos+1);
            s_Server->on_command(this, str);
        }
        else break;
    }
	read_some();
}

void SClient::on_closed()
{
	std::cout << "Socket closed" << endl;
	s_Server->close(this);
}

void SClient::on_connected()
{
	cout << "Accepted new connection!" << endl;
	s_Server->accept(this);
	read_some();
}

