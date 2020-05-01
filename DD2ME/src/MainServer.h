#ifndef MAINSERVER_H
#define MAINSERVER_H

#include "Game.h"

#include "NetServer/Server.h"

class MainServer
{
    public:
        MainServer();
        ~MainServer();
        PostParsingStruct* s_ServerList;
        map<string, Game*> s_ListGameClass;
        map<unsigned int, Game*> s_ListTimerId;
        Server* s_Server;
        bool load();
        bool run();
};

#endif
