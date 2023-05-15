#include "Server.h"

#include "../WorkFunctions.h"
#include "../Defines.h"
#include "../IniParser/ParserInfoFile.h"
#include "../IniParser/PostParsingStruct.h"

#include "../MainServer.h"
#include "../UserData.h"

#include "../NetClient/NetInfoStruct.h"

#include <iostream>

using namespace std;
using namespace irr;
using namespace net;

using namespace WorkFunctions;
using namespace ParserFunctions;
using namespace ConvertFunctions;

using namespace IniParser;

Server::Server(MainServer* mainserver):
    s_MainServer(mainserver)
{
    //...
}

Server::~Server()
{
    //...
}

void Server::sendOutPacket(SOutPacket& outpacket, const s32 playerId, const u32 channelID)
{
    outpacket.compressPacket();
    outpacket.encryptPacket(STRING_CONSTANTS::SC_CRYPT_KEY_NET.c_str());
	s_NetManager->sendOutPacket(outpacket, playerId, channelID);
}

void Server::sendOutPacketUnreliable(SOutPacket& outpacket, const s32 playerId, const u32 channelID, bool isUnsequenced)
{
    outpacket.compressPacket();
    outpacket.encryptPacket(STRING_CONSTANTS::SC_CRYPT_KEY_NET.c_str());
	s_NetManager->sendOutPacketUnreliable(outpacket, playerId, channelID, isUnsequenced);
}

void Server::accept(const int playerId)
{
	s_Clients[playerId] = UserData();

	string str_send = "";
	str_send = addMainVariableString(str_send, "connection", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "confirm", "true", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
	str_send = addMainVariableString(str_send, "SystemInfo", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_ConfirmConnection, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);

	SOutPacket packet;
    packet << str_send;
	sendOutPacket(packet, playerId);
}

void Server::tick()
{
    map<string, Game*>::iterator iter, iter2;
    for (iter = s_MainServer->s_ListGameClass.begin(), iter2 = s_MainServer->s_ListGameClass.end(); iter != iter2;)
    {
        iter->second->processAllEvents(atoi( iter->second->s_NetClient->s_NetInfo->getValue("internet", "maxnumberofeventsatatime").c_str() ));
        iter++;
    }
}

void Server::onCommand(const int playerId, const std::string& cmd)
{
	ParserInfoFile prs;
	PostParsingStruct* pps = prs.getParsedFromString(cmd, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
	if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_CLIENT_IDS_MESSAGES::FCIM_PlayerAuth)
    {
        string str_send;
        str_send = addMainVariableString(str_send, "SystemInfo", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_PlayerAuth, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        bool confirm = false;
        string name = pps->getValue("login", "name");
        string pass = pps->getValue("login", "pass");
        PostParsingStruct* UserIni = prs.getParsedFromFile("ServerData/Users/User_" + name, 0, false);
        if(!UserIni) confirm = false;
        else
        {
            if(name != UserIni->getValue("login", "name") || pass != UserIni->getValue("login", "pass")) confirm = false;
            else confirm = true;
        }
        if(s_MainServer->s_Server->s_ClientsId[UserIni->getValue("login", "name")] != 0) confirm = false;
        if(confirm == false)
        {
            str_send = addMainVariableString(str_send, "login", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
            str_send = addSecondaryVariableString(str_send, "confirm", "false", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        }
        else
        {
            str_send = addMainVariableString(str_send, "login", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
            str_send = addSecondaryVariableString(str_send, "confirm", "true", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        }

        SOutPacket packet;
        packet << str_send;
        sendOutPacket(packet, playerId);

        if(s_Clients[playerId].s_UserInfo != 0) delete s_Clients[playerId].s_UserInfo;
        s_Clients[playerId].s_UserInfo = UserIni;
        s_MainServer->s_Server->s_ClientsId[UserIni->getValue("login", "name")] = playerId;
    }
    if(s_Clients[playerId].s_UserInfo == 0)
    {
        delete pps;
        close(playerId);
        return;
    }
    /*if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_CLIENT_IDS_MESSAGES::FCIM_InfoFromClient && s_Clients[playerId].s_IdServerConnected != STRING_CONSTANTS::MISSING_ID_SERVER)
    {
        //s_MainServer->s_ListGameClass[s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->setKeys(pps, "Keys");
        string playernickname = s_MainServer->s_ListGameClass[s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_NickName;
        s_MainServer->s_ListGameClass[s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->setListOfVariables(pps, "player");
        s_MainServer->s_ListGameClass[s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_NickName = playernickname;
        string str_send = "";
        str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_ConfirmGettingInfoFromClient, SPLITTER_STR_VARIABLE);
        str_send += "\n";
        cl->send(str_send);
    }*/
    if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_CLIENT_IDS_MESSAGES::FCIM_Command)
    {
        doCommand(playerId, pps->getValue("command", "do"), pps);
    }
	delete pps;
}

void Server::doCommand(const int playerId, string command, PostParsingStruct* pps)
{
    string str_send;
    if(command == SERVER_COMMANDS_FROM_CLIENT::SCFC_getServerList)
    {
        str_send = "";
        str_send = addMainVariableString(str_send, "SystemInfo", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_ServerList, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        map<string, map<string, string > >::iterator iter;
        for(iter = s_MainServer->s_ServerList->getMapVariables().begin(); iter != s_MainServer->s_ServerList->getMapVariables().end(); iter++)
        {
            str_send = addMainVariableString(str_send, iter->first, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
            map<string, string>::iterator iter1;
            for(iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++) str_send = addSecondaryVariableString(str_send, iter1->first, iter1->second, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        }

        SOutPacket packet;
        packet << str_send;
        sendOutPacket(packet, playerId);
    }
    /*else if(command == SERVER_COMMANDS_FROM_CLIENT::SCFC_connectToServer)
    {
        string id = pps->getValue("command", "id");
        ParserInfoFile prs;
        if(s_Clients[playerId].s_IdServerConnected != STRING_CONSTANTS::MISSING_ID_SERVER)
        {
            return;
        }
        if(!s_MainServer->s_ListGameClass[id])
        {
            return;
        }
        if(!s_MainServer->s_ListGameClass[id]->insertPlayer(playerId, 1, s_Clients[playerId].s_UserInfo->getValue("login", "name")))
        {
            return;
        }
        str_send = prs.convertPostParsingStructToString(s_MainServer->s_ListGameClass[id]->s_IniFile, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        str_send = addMainVariableString(str_send, "SystemInfo", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_MainIniFile, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);

        SOutPacket packet;
        packet << str_send;
        sendOutPacket(packet, playerId);

        s_Clients[playerId].s_IdServerConnected = id;
    }
    else if(command == SERVER_COMMANDS_FROM_CLIENT::SCFC_getCreaturesList)
    {
        if(s_Clients[playerId].s_IdServerConnected == STRING_CONSTANTS::MISSING_ID_SERVER)
        {
            return;
        }
        string id = pps->getValue("command", "id");
        ParserInfoFile prs;
        PostParsingStruct* cpps = s_MainServer->s_ListGameClass[id]->getObjects();
        str_send = prs.convertPostParsingStructToString(cpps, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        str_send = addMainVariableString(str_send, "SystemInfo", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_ListCreatures, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "MyID", itos(playerId), STRING_CONSTANTS::SPLITTER_STR_VARIABLE);

        SOutPacket packet;
        packet << str_send;
        sendOutPacket(packet, playerId);
    }
    else if(command == SERVER_COMMANDS_FROM_CLIENT::SCFC_leaveServer)
    {
        if(s_Clients[playerId].s_IdServerConnected != STRING_CONSTANTS::MISSING_ID_SERVER) s_MainServer->s_ListGameClass[s_Clients[playerId].s_IdServerConnected]->removePlayer(playerId);
        s_Clients[playerId].s_IdServerConnected = STRING_CONSTANTS::MISSING_ID_SERVER;
        str_send = "";
        str_send = addMainVariableString(str_send, "SystemInfo", STRING_CONSTANTS::SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_ConfirmLeaveServer, STRING_CONSTANTS::SPLITTER_STR_VARIABLE);

        SOutPacket packet;
        packet << str_send;
        sendOutPacket(packet, playerId);
    }*/
}

void Server::close(const int playerId)
{
    if(s_Clients[playerId].s_IdServerConnected != STRING_CONSTANTS::MISSING_ID_SERVER) s_MainServer->s_ListGameClass[s_Clients[playerId].s_IdServerConnected]->removePlayer(playerId);
    s_Clients[playerId].s_IdServerConnected = STRING_CONSTANTS::MISSING_ID_SERVER;
    s_MainServer->s_Server->s_ClientsId[s_Clients[playerId].s_UserInfo->getValue("login", "name")] = 0;
    s_MainServer->s_Server->s_ClientsId.erase(s_Clients[playerId].s_UserInfo->getValue("login", "name"));
	s_Clients.erase(playerId);
}

void Server::run(int port)
{
	std::cout << "Starting server" << std::endl;

	if(s_MainServer->s_ListGameClass.size() == 0)
    {
        cout << "Error: servers is missing!" << endl;
        return;
    }

	SNetParams snp;

	snp.numberChannels = 2;
	snp.maxClients = atoi( s_MainServer->s_ListGameClass.begin()->second->s_NetClient->s_NetInfo->getValue("server", "maxclients").c_str() );
	snp.downBandwidth = atoi( s_MainServer->s_ListGameClass.begin()->second->s_NetClient->s_NetInfo->getValue("internet", "downbandwidth").c_str() );
	snp.upBandwidth = atoi( s_MainServer->s_ListGameClass.begin()->second->s_NetClient->s_NetInfo->getValue("internet", "upbandwidth").c_str() );

	s_NetManager = createIrrNetServer(0, port, snp);
    //s_NetManager->setVerbose(true);
    NetServerCallback* serverCallback = new NetServerCallback(this);
    s_NetManager->setNetCallback(serverCallback);

    while(s_NetManager->getConnectionStatus() != EICS_FAILED)
    {
        s_NetManager->update(5);
        tick();
    }
}



