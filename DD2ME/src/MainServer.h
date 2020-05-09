#ifndef MAINSERVER_H
#define MAINSERVER_H

#include "Game.h"

#include "NetServer/Server.h"

class MainServer
{
    public:
        MainServer();
        ~MainServer();
        IniParser::PostParsingStruct* s_ServerList;
        std::map<std::string, Game*> s_ListGameClass;
        std::map<unsigned int, Game*> s_ListTimerId;
        Server* s_Server;
        bool load();
        bool run();
};

#endif
