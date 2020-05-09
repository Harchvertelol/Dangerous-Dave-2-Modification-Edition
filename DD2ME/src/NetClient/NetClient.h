#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <string>

#include "../shabbynet/shabbynet.hpp"
#include "Client.h"
#include "NetInfoStruct.h"
#include "../IniParser/PostParsingStruct.h"

class Game;

class NetClient
{
    public:
        NetClient(Game*);
        ~NetClient();
        Client* s_Client;
        NetInfoStruct* s_NetInfoStruct;
        Game* s_GameClass;
        PostParsingStruct* s_NetInfo;
        bool connect();
        bool netGameStartWork();
        bool getServerList();
        void getCreaturesList();
        bool choiceServer();
        void sendInfoFromClient();
        void leaveServer();
        void sendCommandToServer(std::string);

    private:
        void tick();
};

#endif

