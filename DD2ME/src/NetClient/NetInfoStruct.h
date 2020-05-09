#ifndef NETINFOSTRUCT_H
#define NETINFOSTRUCT_H

#include <iostream>

#include "../IniParser/PostParsingStruct.h"

namespace SERVER_COMMANDS_FROM_CLIENT
{
    const string SCFC_connectToServer = "connectToServer";
    const string SCFC_getCreaturesList = "getCreaturesList";
    const string SCFC_getServerList = "getServerList";
    const string SCFC_leaveServer = "leaveServer";
}

namespace FROM_CLIENT_IDS_MESSAGES
{
    const string FCIM_PlayerAuth = "PlayerAuth";
    const string FCIM_Command = "Command";
    const string FCIM_InfoFromClient = "InfoFromClient";
}

namespace FROM_SERVER_IDS_MESSAGES
{
    const string FSIM_ConfirmConnection = "ConfirmConnection";
    const string FSIM_PlayerAuth = "PlayerAuth";
    const string FSIM_ServerList = "ServerList";
    const string FSIM_MainIniFile = "MainIniFile";
    const string FSIM_ListCreatures = "ListCreatures";
    const string FSIM_ConfirmGettingInfoFromClient = "ConfirmGettingInfoFromClient";
    const string FSIM_ConfirmLeaveServer = "ConfirmLeaveServer";
    const string FSIM_Command = "Command";
}

struct NetInfoStruct
{
    bool s_goGame;
    bool s_goGameOnServer;
    std::string s_Name;
    std::string s_Pass;
    std::string s_Host;
    int s_Port;
    //...
    bool s_Sleep_1;
    bool s_Sleep_2;
    bool s_Sleep_3;
    bool s_Sleep_4;
    //...
    bool s_SuperSleep_1;
    //...
    std::string s_Error;
    //...
    std::string s_Mode;
    //...
    bool s_WaitingGettingCreatureList;
    bool s_WaitingConfirmGettingInfoFromClient;
    bool s_WaitingConfirmLeaveServer;
    //...
    PostParsingStruct* s_ServerList;
    unsigned int s_ServerIdNow;
};

#endif
