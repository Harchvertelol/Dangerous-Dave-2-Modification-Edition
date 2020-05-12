#ifndef SERVER_H
#define SERVER_H

#include <set>
#include <map>

#include "../shabbynet/shabbynet.hpp"
#include "NetServerCallback.h"
#include "../IniParser/PostParsingStruct.h"

class MainServer;

class Server
{
    public:
        Server(MainServer*);
        ~Server();
        MainServer* s_MainServer;
        void accept(NetServerCallback*);
        void run(int port);
        void close(NetServerCallback*);
        void on_command(NetServerCallback*, const std::string&);
        void doCommand(NetServerCallback*, std::string, IniParser::PostParsingStruct*);

    private:
        void tick();
        std::map<std::string, int> s_ClientsId;
        std::map<int, NetServerCallback*> s_Clients;
        int s_MaxIdClient;
};

#endif // SERVER_H

