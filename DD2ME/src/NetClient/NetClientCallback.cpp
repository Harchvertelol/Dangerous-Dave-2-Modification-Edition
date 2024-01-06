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
    else if(channelID == 2)
    {
        int packet_type;
        packet >> packet_type;
        if(s_NetClient->s_NetInfoStruct->s_goGameOnServer)
        {
            if(packet_type == PT_PLAYER_COORDS)
            {
                int pl_id, new_x, new_y;
                packet >> pl_id;
                if(pl_id == s_NetClient->s_MyID || s_NetClient->s_MyID == -1) return;
                packet >> new_x;
                packet >> new_y;
                if(s_NetClient->s_GameClass->s_GameInfo->s_Players.find(pl_id) != s_NetClient->s_GameClass->s_GameInfo->s_Players.end())
                {
                    s_NetClient->s_GameClass->s_GameInfo->s_Players[pl_id]->s_CoordX = new_x;
                    s_NetClient->s_GameClass->s_GameInfo->s_Players[pl_id]->s_CoordY = new_y;
                }
            }
            else if(packet_type == PT_PLAYER_CONNECTED)
            {
                int pl_id;
                packet >> pl_id;
                if(pl_id == s_NetClient->s_MyID || s_NetClient->s_MyID == -1) return;
                string str;
                packet >> str;
                if(s_NetClient->s_GameClass->s_GameInfo->s_Players.find(pl_id) != s_NetClient->s_GameClass->s_GameInfo->s_Players.end()) delete s_NetClient->s_GameClass->s_GameInfo->s_Players[pl_id];
                s_NetClient->s_GameClass->insertPlayer(pl_id, 1, "", false);
                ParserInfoFile prs;
                PostParsingStruct* dpps = prs.getParsedFromString(str, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
                s_NetClient->s_GameClass->s_GameInfo->s_Players[pl_id]->setListOfVariables(dpps, "player");
                delete dpps;
            }
            else if(packet_type == PT_PLAYER_STATE)
            {
                int pl_id;
                packet >> pl_id;
                if(pl_id == s_NetClient->s_MyID || s_NetClient->s_MyID == -1) return;
                string state, statebod;
                int cartridges, numberoa, addnumberoa, oldnumberoa, shootnow;
                packet >> state;
                packet >> statebod;
                packet >> cartridges;
                packet >> numberoa;
                packet >> addnumberoa;
                packet >> oldnumberoa;
                packet >> shootnow;
                if(s_NetClient->s_GameClass->s_GameInfo->s_Players.find(pl_id) != s_NetClient->s_GameClass->s_GameInfo->s_Players.end())
                {
                    s_NetClient->s_GameClass->s_GameInfo->s_Players[pl_id]->s_State = state;
                    s_NetClient->s_GameClass->s_GameInfo->s_Players[pl_id]->s_StateBeforeOpenDoor = statebod;
                    s_NetClient->s_GameClass->s_GameInfo->s_Players[pl_id]->s_Cartridges = cartridges;
                    //s_NetClient->s_GameClass->s_GameInfo->s_Players[pl_id]->s_NumberOfAction = numberoa;
                    s_NetClient->s_GameClass->s_GameInfo->s_Players[pl_id]->s_AdditionalNumberOfAction = addnumberoa;
                    s_NetClient->s_GameClass->s_GameInfo->s_Players[pl_id]->s_OldNumberOfAction = oldnumberoa;
                    s_NetClient->s_GameClass->s_GameInfo->s_Players[pl_id]->s_ShootNow = shootnow;
                }
            }
        }
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
        int MyID = atoi( pps->getValue("SystemInfo", "MyID").c_str() );
        string params = pps->getValue("SystemInfo", "params");
        bool notfullfornetmode = false;
        if(params == "notfullfornetmode") notfullfornetmode = true;
        if(notfullfornetmode) prs.writeParsedToFile(pps, "TECHNICAL.txt");
        s_NetClient->s_GameClass->setObjects(pps, notfullfornetmode);
        if(!notfullfornetmode)
        {
            //s_NetClient->s_GameClass->s_GameInfo->s_MyPlayer->mergePlayer(s_NetClient->s_GameClass->s_GameInfo->s_Players[MyID]);
            s_NetClient->s_GameClass->s_GameInfo->s_MyPlayer->s_NickName = s_NetClient->s_GameClass->s_GameInfo->s_Players[MyID]->s_NickName;
            s_NetClient->s_GameClass->removePlayer(MyID);
        }
        s_NetClient->s_NetInfoStruct->s_WaitingGettingCreatureList = false;
        s_NetClient->s_MyID = MyID;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_SERVER_IDS_MESSAGES::FSIM_ConfirmGettingFullInfoFromClient)
    {
        s_NetClient->s_NetInfoStruct->s_WaitingConfirmGettingFullInfoFromClient = false;
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

