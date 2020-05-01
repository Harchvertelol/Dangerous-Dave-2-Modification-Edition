#ifndef CLIENT_H
#define CLIENT_H

#include <string>

using namespace std;

#include "../shabbynet/shabbynet.hpp"

#include "../IniParser/PostParsingStruct.h"

class NetClient;

class Client: public Socket
{
    public:
        Client(NetClient*);
        ~Client();
        NetClient* s_NetClient;
        string s_BalanceStr;
        void workStr(string);
        void doCommand(Client*,string,PostParsingStruct*);
        void connect(const std::string&, int port);

    private:
        void on_received(const char*, int);
};

#endif // CLIENT_H


