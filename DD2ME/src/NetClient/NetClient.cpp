#include "NetClient.h"
#include <iostream>
#include <sstream>
#include <cstdlib>

#include "../Game.h"
#include "../WorkFunction.h"
#include "../IniParser/ParserInfoFile.h"
#include "../IniParser/PostParsingStruct.h"

#include "../Defines.h"

using namespace std;

using namespace WorkFunction;
using namespace ParserFunction;
using namespace STRING_CONSTANTS;

NetClient::NetClient(Game* gameclass):
    s_GameClass(gameclass),
    s_NetInfo(0)
{
    s_Client = new Client(this);
    s_NetInfoStruct = new NetInfoStruct;
    s_NetInfoStruct->s_Error = "";
    s_NetInfoStruct->s_ServerList = 0;
    s_NetInfoStruct->s_ServerIdNow = 0;
    s_NetInfoStruct->s_WaitingConfirmGettingInfoFromClient = false;
    s_NetInfoStruct->s_WaitingGettingCreatureList = false;
    s_NetInfoStruct->s_WaitingConfirmLeaveServer = false;
}

NetClient::~NetClient()
{
    if(s_Client != 0) delete s_Client;
    if(s_NetInfoStruct != 0) delete s_NetInfoStruct;
    if(s_NetInfo != 0) delete s_NetInfo;
}

void NetClient::tick()
{
	//...
}

bool NetClient::connect()
{
    try
    {
        s_Client->connect(s_NetInfoStruct->s_Host, s_NetInfoStruct->s_Port);
        cout<<"Connected"<<endl;
    }
    catch(std::exception& e)
	{
		cout << "Error: " << e.what() << endl;
		return false;
	}
	s_NetInfoStruct->s_Sleep_1 = true;
	cout<<"Waiting confirm..."<<endl;
	for(int i = 0; s_NetInfoStruct->s_Sleep_1 == true; i++)
    {
        net::run(1000);
        cout<<".";
        if(i == 10)
        {
            cout<<endl<<"Error: Not confirm."<<endl;
            return false;
        }
    }
    cout<<endl<<"Confirmed."<<endl;
    cout<<"Authorization..."<<endl;
	string str_send = "";
	str_send = addMainVariableString(str_send, "login", SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "name", s_NetInfoStruct->s_Name, SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "pass", s_NetInfoStruct->s_Pass, SPLITTER_STR_VARIABLE);
	str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_CLIENT_IDS_MESSAGES::FCIM_PlayerAuth, SPLITTER_STR_VARIABLE);
	str_send += "\n";
	s_Client->send(str_send);
	str_send = "";
	s_NetInfoStruct->s_Sleep_2 = true;
	for(int i = 0; s_NetInfoStruct->s_Sleep_2 == true; i++)
    {
        net::run(1000);
        cout<<".";
        if(i == 10)
        {
            cout<<endl<<"Error: Server timeout."<<endl;
            return false;
        }
    }
    if(s_NetInfoStruct->s_Error != "")
    {
        cout<<"Error: "<<s_NetInfoStruct->s_Error<<endl;
        s_NetInfoStruct->s_Error = "";
        return false;
    }
    cout<<"You are logged."<<endl;
	return true;
}

bool NetClient::netGameStartWork()
{
    if(!getServerList())
    {
        cout<<"Error getting server list."<<endl;
        return false;
    }
    if(!choiceServer())
    {
        cout<<"Error choicing server."<<endl;
        return false;
    }
    return true;
}

bool NetClient::getServerList()
{
    string str_send;
    cout<<"Getting server list..."<<endl;
    sendCommandToServer(SERVER_COMMANDS_FROM_CLIENT::SCFC_getServerList);
    s_NetInfoStruct->s_Sleep_3 = true;
    for(int i = 0; s_NetInfoStruct->s_Sleep_3 == true; i++)
    {
        net::run(1000);
        cout<<".";
        if(i == atoi( s_NetInfo->getValue("internet", "timeoutconnect").c_str() ) )
        {
            cout<<endl<<"Error: Server timeout."<<endl;
            return false;
        }
    }
    cout<<"Server list getted."<<endl;
    return true;
}

bool NetClient::choiceServer()
{
    int num, number = 0;
    cout<<"Choice server please (number) or type '0' for exit: ";
    cin>>num;
    if(num == 0)
    {
        cout<<"Exiting..."<<endl;
        return false;
    }
    map<string, map<string, string > >::iterator iter;
    for(iter = s_NetInfoStruct->s_ServerList->s_Variables.begin(); iter != s_NetInfoStruct->s_ServerList->s_Variables.end(); iter++)
    {
        if(iter->first.find("Server") == 0) number++;
        if(number == num) break;
    }
    if(number != num)
    {
        cout<<"Number of server is bad."<<endl;
        return false;
    }
    s_NetInfoStruct->s_ServerIdNow = atoi( s_NetInfoStruct->s_ServerList->getValue(iter->first, "id").c_str() );
    sendCommandToServer(SERVER_COMMANDS_FROM_CLIENT::SCFC_connectToServer);
	cout<<"Connecting to game server..."<<endl;
	s_NetInfoStruct->s_SuperSleep_1 = true;
	while(s_NetInfoStruct->s_SuperSleep_1 == true)
    {
        s_NetInfoStruct->s_Sleep_4 = true;
        for(int i = 0; s_NetInfoStruct->s_Sleep_4 == true; i++)
        {
            net::run(1000);
            cout<<".";
            if(i == atoi( s_NetInfo->getValue("internet", "timeoutconnect").c_str() ) )
            {
                cout<<endl<<"Error: Server timeout."<<endl;
                return false;
            }
        }
    }
	return true;
}

void NetClient::getCreaturesList()
{
    sendCommandToServer(SERVER_COMMANDS_FROM_CLIENT::SCFC_getCreaturesList);
	s_NetInfoStruct->s_WaitingGettingCreatureList = true;
}

void NetClient::sendInfoFromClient()
{
    //PostParsingStruct* pps = s_GameClass->s_GameInfo->s_MyDave->getKeys("Keys");
    //s_GameClass->s_GameInfo->s_MyDave->getListOfVariables("dave", pps);
    PostParsingStruct* pps = s_GameClass->s_GameInfo->s_MyDave->getListOfVariables("dave");
    ParserInfoFile prs;
    string str_send = prs.convertPostParsingStructToString(pps, SPLITTER_STR_VARIABLE);
    str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
    str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_CLIENT_IDS_MESSAGES::FCIM_InfoFromClient, SPLITTER_STR_VARIABLE);
	str_send += "\n";
	s_Client->send(str_send);
	s_NetInfoStruct->s_WaitingConfirmGettingInfoFromClient = true;
}

void NetClient::leaveServer()
{
    sendCommandToServer(SERVER_COMMANDS_FROM_CLIENT::SCFC_leaveServer);
	s_NetInfoStruct->s_WaitingConfirmLeaveServer = true;
}

void NetClient::sendCommandToServer(string command)
{
    string str_send = "";
    str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
    str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_CLIENT_IDS_MESSAGES::FCIM_Command, SPLITTER_STR_VARIABLE);
    str_send = addMainVariableString(str_send, "command", SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "do", command, SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "id", WorkFunction::ConvertFunction::itos(s_NetInfoStruct->s_ServerIdNow), SPLITTER_STR_VARIABLE);
	str_send += "\n";
	s_Client->send(str_send);
}
