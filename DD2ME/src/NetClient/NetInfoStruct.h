#ifndef NETINFOSTRUCT_H
#define NETINFOSTRUCT_H

#include <iostream>

#include "../IniParser/PostParsingStruct.h"

using namespace std;

struct NetInfoStruct
{
    bool s_goGame;
    bool s_goGameOnServer;
    string s_Name;
    string s_Pass;
    string s_Host;
    int s_Port;
    //...
    bool s_Sleep_1;
    bool s_Sleep_2;
    bool s_Sleep_3;
    bool s_Sleep_4;
    //...
    bool s_SuperSleep_1;
    //...
    string s_Error;
    //...
    string s_Mode;
    //...
    bool s_WaitingGettingCreatureList;
    bool s_WaitingConfirmGettingKeysState;
    bool s_WaitingConfirmLeaveServer;
    //...
    PostParsingStruct* s_ServerList;
    unsigned int s_ServerIdNow;
};

#endif
