#ifndef SERVER_H
#define SERVER_H

#include <set>
#include <map>

#include "../shabbynet/shabbynet.hpp"
#include "SClient.h"
#include "../IniParser/PostParsingStruct.h"

class MainServer;

class Server
{
    public:
        Server(MainServer*);
        ~Server();
        MainServer* s_MainServer;
        void accept(SClient*);
        void run(int port);
        void close(SClient*);
        void on_command(SClient*, const std::string&);
        void doCommand(SClient*, std::string, IniParser::PostParsingStruct*);

    private:
        void tick();
        std::map<std::string, int> s_ClientsId;
        std::map<int, SClient*> s_Clients;
        int s_MaxIdClient;
};

#endif // SERVER_H

