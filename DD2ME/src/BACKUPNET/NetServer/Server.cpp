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
using namespace WorkFunctions;
using namespace ParserFunctions;
using namespace ConvertFunctions;
using namespace STRING_CONSTANTS;

using namespace IniParser;

Server::Server(MainServer* mainserver):
    s_MainServer(mainserver),
    s_MaxIdClient(10)
{
    //...
}

Server::~Server()
{
    //...
}

void Server::accept(NetServerCallback* c)
{
	cout << "New client accepted" << endl;
	s_Clients[s_MaxIdClient] = c;
	c->s_ID = s_MaxIdClient;
	s_MaxIdClient++;
	net::accept(new NetServerCallback(this));
	string str_send = "";
	str_send = addMainVariableString(str_send, "connection", SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "confirm", "true", SPLITTER_STR_VARIABLE);
	str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_ConfirmConnection, SPLITTER_STR_VARIABLE);
	str_send += "\n";
	c->send(str_send);
	str_send = "";
}

void Server::tick()
{
	/*Message m;
    if(m.peek())
    {
        if(m.is_timer())
        {
            int timerid = m.timer_id();
            if(s_MainServer->s_ListTimerId[timerid] != 0) s_MainServer->s_ListTimerId[timerid]->onTimer(m.timer_id());
        }
    }*/
    map<string, Game*>::iterator iter, iter2;
    for (iter = s_MainServer->s_ListGameClass.begin(), iter2 = s_MainServer->s_ListGameClass.end(); iter != iter2;)
    {
        iter->second->processAllEvents(atoi( iter->second->s_NetClient->s_NetInfo->getValue("internet", "maxnumberofeventsatatime").c_str() ));
        iter++;
    }
}

void Server::on_command(NetServerCallback* cl, const std::string& cmd)
{
	ParserInfoFile prs;
	PostParsingStruct* pps = prs.getParsedFromString(cmd, SPLITTER_STR_VARIABLE);
	if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_CLIENT_IDS_MESSAGES::FCIM_PlayerAuth)
    {
        string str_send;
        str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_PlayerAuth, SPLITTER_STR_VARIABLE);
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
            str_send = addMainVariableString(str_send, "login", SPLITTER_STR_VARIABLE);
            str_send = addSecondaryVariableString(str_send, "confirm", "false", SPLITTER_STR_VARIABLE);
        }
        else
        {
            str_send = addMainVariableString(str_send, "login", SPLITTER_STR_VARIABLE);
            str_send = addSecondaryVariableString(str_send, "confirm", "true", SPLITTER_STR_VARIABLE);
        }
        str_send += "\n";
        cl->send(str_send);
        cl->s_UserData->s_UserInfo = UserIni;
        s_MainServer->s_Server->s_ClientsId[UserIni->getValue("login", "name")] = cl->s_ID;
    }
    if(cl->s_UserData->s_UserInfo == 0)
    {
        delete pps;
        close(cl);
        return;
    }
    if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_CLIENT_IDS_MESSAGES::FCIM_InfoFromClient && cl->s_UserData->s_IdServerConnected != STRING_CONSTANTS::MISSING_ID_SERVER)
    {
        //s_MainServer->s_ListGameClass[cl->s_UserData->s_IdServerConnected]->s_GameInfo->s_Players[cl->s_ID]->setKeys(pps, "Keys");
        string playernickname = s_MainServer->s_ListGameClass[cl->s_UserData->s_IdServerConnected]->s_GameInfo->s_Players[cl->s_ID]->s_NickName;
        s_MainServer->s_ListGameClass[cl->s_UserData->s_IdServerConnected]->s_GameInfo->s_Players[cl->s_ID]->setListOfVariables(pps, "player");
        s_MainServer->s_ListGameClass[cl->s_UserData->s_IdServerConnected]->s_GameInfo->s_Players[cl->s_ID]->s_NickName = playernickname;
        string str_send = "";
        str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_ConfirmGettingInfoFromClient, SPLITTER_STR_VARIABLE);
        str_send += "\n";
        cl->send(str_send);
    }
    if(pps->getValue("SystemInfo", "ID_MESSAGE") == FROM_CLIENT_IDS_MESSAGES::FCIM_Command)
    {
        doCommand(cl, pps->getValue("command", "do"), pps);
    }
	delete pps;
}

void Server::doCommand(NetServerCallback* cl, string command, PostParsingStruct* pps)
{
    string str_send;
    if(command == SERVER_COMMANDS_FROM_CLIENT::SCFC_getServerList)
    {
        str_send = "";
        str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_ServerList, SPLITTER_STR_VARIABLE);
        map<string, map<string, string > >::iterator iter;
        for(iter = s_MainServer->s_ServerList->getMapVariables().begin(); iter != s_MainServer->s_ServerList->getMapVariables().end(); iter++)
        {
            str_send = addMainVariableString(str_send, iter->first, SPLITTER_STR_VARIABLE);
            map<string, string>::iterator iter1;
            for(iter1 = iter->second.begin(); iter1 != iter->second.end(); iter1++)
            {
                str_send = addSecondaryVariableString(str_send, iter1->first, iter1->second, SPLITTER_STR_VARIABLE);
            }
        }
        str_send += "\n";
        cl->send(str_send);
    }
    else if(command == SERVER_COMMANDS_FROM_CLIENT::SCFC_connectToServer)
    {
        string id = pps->getValue("command", "id");
        ParserInfoFile prs;
        if(cl->s_UserData->s_IdServerConnected != STRING_CONSTANTS::MISSING_ID_SERVER)
        {
            return;
        }
        if(!s_MainServer->s_ListGameClass[id])
        {
            return;
        }
        if(!s_MainServer->s_ListGameClass[id]->insertPlayer(cl->s_ID, 1, cl->s_UserData->s_UserInfo->getValue("login", "name")))
        {
            return;
        }
        str_send = prs.convertPostParsingStructToString(s_MainServer->s_ListGameClass[id]->s_IniFile, SPLITTER_STR_VARIABLE);
        str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_MainIniFile, SPLITTER_STR_VARIABLE);
        str_send += "\n";
        cl->send(str_send);
        cl->s_UserData->s_IdServerConnected = id;
    }
    else if(command == SERVER_COMMANDS_FROM_CLIENT::SCFC_getCreaturesList)
    {
        if(cl->s_UserData->s_IdServerConnected == STRING_CONSTANTS::MISSING_ID_SERVER)
        {
            return;
        }
        string id = pps->getValue("command", "id");
        ParserInfoFile prs;
        PostParsingStruct* cpps = s_MainServer->s_ListGameClass[id]->getObjects();
        str_send = prs.convertPostParsingStructToString(cpps, SPLITTER_STR_VARIABLE);
        str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_ListCreatures, SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "MyID", itos(cl->s_ID), SPLITTER_STR_VARIABLE);
        str_send += "\n";
        cl->send(str_send);
    }
    else if(command == SERVER_COMMANDS_FROM_CLIENT::SCFC_leaveServer)
    {
        if(cl->s_UserData->s_IdServerConnected != STRING_CONSTANTS::MISSING_ID_SERVER) s_MainServer->s_ListGameClass[cl->s_UserData->s_IdServerConnected]->removePlayer(cl->s_ID);
        cl->s_UserData->s_IdServerConnected = STRING_CONSTANTS::MISSING_ID_SERVER;
        str_send = "";
        str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
        str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_SERVER_IDS_MESSAGES::FSIM_ConfirmLeaveServer, SPLITTER_STR_VARIABLE);
        str_send += "\n";
        cl->send(str_send);
    }
}

void Server::close(NetServerCallback* c)
{
    if(c->s_UserData->s_IdServerConnected != STRING_CONSTANTS::MISSING_ID_SERVER) s_MainServer->s_ListGameClass[c->s_UserData->s_IdServerConnected]->removePlayer(c->s_ID);
    c->s_UserData->s_IdServerConnected = STRING_CONSTANTS::MISSING_ID_SERVER;
    s_MainServer->s_Server->s_ClientsId[c->s_UserData->s_UserInfo->getValue("login", "name")] = 0;
	s_Clients.erase(c->s_ID);
	delete c;
}

void Server::run(int port)
{
	std::cout << "Starting server" << std::endl;
	net::listen(port);

	net::accept(new NetServerCallback(this));
	for(;;)
	{
		net::run(5);
		tick();
	}
}



