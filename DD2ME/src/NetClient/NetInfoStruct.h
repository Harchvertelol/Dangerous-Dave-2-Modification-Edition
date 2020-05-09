#ifndef NETINFOSTRUCT_H
#define NETINFOSTRUCT_H

#include <iostream>

#include "../IniParser/PostParsingStruct.h"

namespace SERVER_COMMANDS_FROM_CLIENT
{
    const std::string SCFC_connectToServer = "connectToServer";
    const std::string SCFC_getCreaturesList = "getCreaturesList";
    const std::string SCFC_getServerList = "getServerList";
    const std::string SCFC_leaveServer = "leaveServer";
}

namespace FROM_CLIENT_IDS_MESSAGES
{
    const std::string FCIM_PlayerAuth = "PlayerAuth";
    const std::string FCIM_Command = "Command";
    const std::string FCIM_InfoFromClient = "InfoFromClient";
}

namespace FROM_SERVER_IDS_MESSAGES
{
    const std::string FSIM_ConfirmConnection = "ConfirmConnection";
    const std::string FSIM_PlayerAuth = "PlayerAuth";
    const std::string FSIM_ServerList = "ServerList";
    const std::string FSIM_MainIniFile = "MainIniFile";
    const std::string FSIM_ListCreatures = "ListCreatures";
    const std::string FSIM_ConfirmGettingInfoFromClient = "ConfirmGettingInfoFromClient";
    const std::string FSIM_ConfirmLeaveServer = "ConfirmLeaveServer";
    const std::string FSIM_Command = "Command";
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
    IniParser::PostParsingStruct* s_ServerList;
    unsigned int s_ServerIdNow;
};

#endif
