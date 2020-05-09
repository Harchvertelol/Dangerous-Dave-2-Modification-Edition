#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include "../shabbynet/shabbynet.hpp"

#include "../IniParser/PostParsingStruct.h"

class NetClient;

class Client: public Socket
{
    public:
        Client(NetClient*);
        ~Client();
        NetClient* s_NetClient;
        std::string s_ReceiveBuffer;
        void workStr(std::string);
        void doCommand(Client*, std::string, IniParser::PostParsingStruct*);
        void connect(const std::string&, int port);

    private:
        void on_received(const char*, int);
};

#endif // CLIENT_H


