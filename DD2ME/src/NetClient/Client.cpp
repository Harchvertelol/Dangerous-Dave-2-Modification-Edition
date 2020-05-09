#include "Client.h"
#include <iostream>
#include <string>

#include "../WorkFunction.h"
#include "../IniParser/ParserInfoFile.h"
#include "../IniParser/PostParsingStruct.h"
#include "NetClient.h"

#include "../Defines.h"

#include "../Game.h"

using namespace std;

using namespace WorkFunction;
using namespace ParserFunction;
using namespace STRING_CONSTANTS;

Client::Client(NetClient* g):
    s_NetClient(g),
    s_ReceiveBuffer("")
{
    //...
}

Client::~Client()
{
    //...
}

void Client::connect(const std::string& host, int port)
{
	Socket::connect(host, port);
	read_some();
}

void Client::on_received(const char* buf, int len)
{
	if(len < 0)
    {
        cout<<"Error! Length < 0."<<endl;
        read_some();
        return;
    }
    string str, buffer;
    buffer.append(buf, len);
    buffer = s_ReceiveBuffer + buffer;
    for(;;) // ����� � ��� ��������� �����
    {
        const size_t pos = buffer.find('\n');
        if(pos != string::npos) // ���� ������
        {
            str = buffer.substr(0, pos);
            buffer.erase(0, pos+1);
            workStr(str);
        }
        else break;
    }
    s_ReceiveBuffer = buffer;
	read_some();
}

void Client::workStr(string s)
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
        for(iter = pps->s_Variables.begin(); iter != pps->s_Variables.end(); iter++)
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
        //s_NetClient->s_GameClass->s_GameInfo->s_MyDave->mergeDave(s_NetClient->s_GameClass->s_GameInfo->s_Daves[MyID]);
        s_NetClient->s_GameClass->s_GameInfo->s_MyDave->s_NickName = s_NetClient->s_GameClass->s_GameInfo->s_Daves[MyID]->s_NickName;
        s_NetClient->s_GameClass->removeDave(MyID);
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

void Client::doCommand(Client* cl, string command, PostParsingStruct* pps)
{
    //...
}

