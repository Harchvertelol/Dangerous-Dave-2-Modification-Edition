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
    const std::string FCIM_FullInfoFromClient = "FullInfoFromClient";
}

namespace FROM_SERVER_IDS_MESSAGES
{
    const std::string FSIM_ConfirmConnection = "ConfirmConnection";
    const std::string FSIM_PlayerAuth = "PlayerAuth";
    const std::string FSIM_ServerList = "ServerList";
    const std::string FSIM_MainIniFile = "MainIniFile";
    const std::string FSIM_ListCreatures = "ListCreatures";
    const std::string FSIM_ConfirmGettingFullInfoFromClient = "ConfirmGettingFullInfoFromClient";
    const std::string FSIM_ConfirmLeaveServer = "ConfirmLeaveServer";
    const std::string FSIM_Command = "Command";
}

enum PACKETS_TYPES
{
    PT_PLAYER_COORDS = 1,
    PT_PLAYER_CONNECTED,
    PT_PLAYER_STATE,
    PT_PLAYER_KEYS_STATE,
    PT_PLAYER_DISCONNECTED_FROM_SERVER,
    PT_OPEN_DOOR,
    PT_SET_TILE_ID
};

enum NET_MODE
{
    NM_NOTDEFINED = 0,
    NM_SINGLEPLAYER,
    NM_MULTIPLAYER,
    NM_SERVER
};

struct NetInfoStruct
{
    bool s_goGame = false;
    bool s_goGameOnServer = false;
    std::string s_Name = "";
    std::string s_Pass = "";
    std::string s_Host = "";
    int s_Port = -1;
    //...
    bool s_Sleep_1 = false;
    bool s_Sleep_2 = false;
    bool s_Sleep_3 = false;
    bool s_Sleep_4 = false;
    //...
    bool s_SuperSleep_1 = false;
    //...
    std::string s_Error = "";
    //...
    NET_MODE s_Mode = NM_NOTDEFINED;
    void* s_Server = 0;
    //...
    bool s_WaitingGettingCreatureList = false;
    bool s_WaitingConfirmGettingFullInfoFromClient = false;
    bool s_WaitingConfirmLeaveServer = false;
    //...
    IniParser::PostParsingStruct* s_ServerList = 0;
    unsigned int s_ServerIdNow = -1;
};

#endif
