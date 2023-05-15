#include "NetClientCallback.h"
#include <iostream>
#include <string>

#include "../WorkFunctions.h"
#include "../IniParser/ParserInfoFile.h"
#include "../IniParser/PostParsingStruct.h"
#include "NetClient.h"

#include "../Defines.h"

#include "../Game.h"

using namespace std;

using namespace irr;
using namespace net;

using namespace WorkFunctions;
using namespace ParserFunctions;
using namespace STRING_CONSTANTS;

using namespace IniParser;

NetClientCallback::NetClientCallback(NetClient* g):
    s_NetClient(g)
{
    //...
}

NetClientCallback::~NetClientCallback()
{
    //...
}

void NetClientCallback::onConnect(const u16 playerId)
{
	cout << "Connected to server" << endl;
}

void NetClientCallback::onDisconnect(const u16 playerId)
{
    cout << "Disconnected from server" << endl;
}

void NetClientCallback::handlePacket(SInPacket& packet, u32 channelID)
{
	packet.decryptPacket(STRING_CONSTANTS::SC_CRYPT_KEY_NET.c_str());
    packet.deCompressPacket();

    if(channelID == 0)
    {
        string str;
        packet >> str;
        workStr(str);
    }
}

void NetClientCallback::workStr(string s)
{
    ParserInfoFile prs;
    PostParsingStruct* pps = prs.getParsedFromString(s, SPLITTER_STR_VARIABLE);
    if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_SERVER_IDS_MESSAGES::FSIM_ConfirmConnection)
    {
        if(pps->getValue("connection", "confirm") == "true") s_NetClient->s_NetInfoStruct->s_Sleep_1 = false;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_SERVER_IDS_MESSAGES::FSIM_PlayerAuth)
    {
        s_NetClient->s_NetInfoStruct->s_Sleep_2 = false;
        if(pps->getValue("login", "confirm") != "true") s_NetClient->s_NetInfoStruct->s_Error = "Invalid username or password. Or player with this name online";
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_SERVER_IDS_MESSAGES::FSIM_ServerList)
    {
        s_NetClient->s_NetInfoStruct->s_Sleep_3 = false;
        int number = 1;
        map<string, map<string, string > >::iterator iter;
        for(iter = pps->getMapVariables().begin(); iter != pps->getMapVariables().end(); iter++)
        {
            if(iter->first.find("Server") == 0)
            {
                cout << number << ") " << "Server Name: " << pps->getValue(iter->first, "name")<< ". ModPack: " << pps->getValue(iter->first, "modpack") << "."<<endl;
                number++;
            }
        }
        if(s_NetClient->s_NetInfoStruct->s_ServerList != 0) delete s_NetClient->s_NetInfoStruct->s_ServerList;
        s_NetClient->s_NetInfoStruct->s_ServerList = pps;
        return;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_SERVER_IDS_MESSAGES::FSIM_MainIniFile)
    {
        if(s_NetClient->s_GameClass->s_IniFile != 0) delete s_NetClient->s_GameClass->s_IniFile;
        s_NetClient->s_GameClass->s_IniFile = pps;
        s_NetClient->s_NetInfoStruct->s_SuperSleep_1 = false;
        s_NetClient->s_NetInfoStruct->s_Sleep_4 = false;
        return;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_SERVER_IDS_MESSAGES::FSIM_ListCreatures)
    {
        s_NetClient->s_GameClass->setObjects(pps);
        int MyID = atoi( pps->getValue("SystemInfo", "MyID").c_str() );
        //s_NetClient->s_GameClass->s_GameInfo->s_MyPlayer->mergePlayer(s_NetClient->s_GameClass->s_GameInfo->s_Players[MyID]);
        s_NetClient->s_GameClass->s_GameInfo->s_MyPlayer->s_NickName = s_NetClient->s_GameClass->s_GameInfo->s_Players[MyID]->s_NickName;
        s_NetClient->s_GameClass->removePlayer(MyID);
        s_NetClient->s_NetInfoStruct->s_WaitingGettingCreatureList = false;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_SERVER_IDS_MESSAGES::FSIM_ConfirmGettingInfoFromClient)
    {
        s_NetClient->s_NetInfoStruct->s_WaitingConfirmGettingInfoFromClient = false;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_SERVER_IDS_MESSAGES::FSIM_ConfirmLeaveServer)
    {
        s_NetClient->s_NetInfoStruct->s_WaitingConfirmLeaveServer = false;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_SERVER_IDS_MESSAGES::FSIM_Command)
    {
        doCommand(this, pps->getValue("command", "do"), pps);
    }
    delete pps;
}

void NetClientCallback::doCommand(NetClientCallback* cl, string command, PostParsingStruct* pps)
{
    //...
}

