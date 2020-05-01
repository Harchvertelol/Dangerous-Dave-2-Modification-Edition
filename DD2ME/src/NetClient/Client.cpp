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
    s_BalanceStr("")
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
    buffer = s_BalanceStr + buffer;
    for(;;) // вдруг у нас несколько строк
    {
        const size_t pos = buffer.find('\n');
        if(pos != string::npos) // есть строка
        {
            str = buffer.substr(0, pos);
            buffer.erase(0, pos+1);
            workStr(str);
        }
        else break;
    }
    s_BalanceStr = buffer;
	read_some();
}

void Client::workStr(string s)
{
    ParserInfoFile prs;
    PostParsingStruct* pps = prs.getParsedFromString(s, SPLITTER_STR_VARIABLE);
    if(pps->getValue("SystemInfo", "ID_MESSAGE") == "ConfirmConnection")
    {
        if(pps->getValue("connection", "confirm") == "true") s_NetClient->s_NetInfoStruct->s_Sleep_1 = false;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == "PlayerAuth")
    {
        s_NetClient->s_NetInfoStruct->s_Sleep_2 = false;
        if(pps->getValue("login", "confirm") != "true") s_NetClient->s_NetInfoStruct->s_Error = "Invalid username or password. Or player with this name online";
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == "ServerList")
    {
        s_NetClient->s_NetInfoStruct->s_Sleep_3 = false;
        int number = 1;
        map<string, map<string, string > >::iterator iter;
        for(iter = pps->s_Variables.begin(); iter != pps->s_Variables.end(); iter++)
        {
            if(iter->first.find("Server") == 0)
            {
                cout<<number<<") "<<"Server Name: "<<pps->getValue(iter->first, "name")<<". ModPack: "<<pps->getValue(iter->first, "modpack")<<"."<<endl;
                number++;
            }
        }
        if(s_NetClient->s_NetInfoStruct->s_ServerList != 0) delete s_NetClient->s_NetInfoStruct->s_ServerList;
        s_NetClient->s_NetInfoStruct->s_ServerList = pps;
        return;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == "MainIniFile")
    {
        if(s_NetClient->s_GameClass->s_IniFile != 0) delete s_NetClient->s_GameClass->s_IniFile;
        s_NetClient->s_GameClass->s_IniFile = pps;
        //...
        string modpack = s_NetClient->s_NetInfoStruct->s_ServerList->getValue("Server" + WorkFunction::ConvertFunction::itos(s_NetClient->s_NetInfoStruct->s_ServerIdNow), "modpack");
        string texturepack = s_NetClient->s_NetInfoStruct->s_ServerList->getValue("Server" + WorkFunction::ConvertFunction::itos(s_NetClient->s_NetInfoStruct->s_ServerIdNow), "texturepack");
        string monsterpack = s_NetClient->s_NetInfoStruct->s_ServerList->getValue("Server" + WorkFunction::ConvertFunction::itos(s_NetClient->s_NetInfoStruct->s_ServerIdNow), "monsterpack");
        string bonuspack = s_NetClient->s_NetInfoStruct->s_ServerList->getValue("Server" + WorkFunction::ConvertFunction::itos(s_NetClient->s_NetInfoStruct->s_ServerIdNow), "bonuspack");
        string screenpack = s_NetClient->s_NetInfoStruct->s_ServerList->getValue("Server" + WorkFunction::ConvertFunction::itos(s_NetClient->s_NetInfoStruct->s_ServerIdNow), "screenpack");
        string soundpack = s_NetClient->s_NetInfoStruct->s_ServerList->getValue("Server" + WorkFunction::ConvertFunction::itos(s_NetClient->s_NetInfoStruct->s_ServerIdNow), "soundpack");
        string levelpack = s_NetClient->s_NetInfoStruct->s_ServerList->getValue("Server" + WorkFunction::ConvertFunction::itos(s_NetClient->s_NetInfoStruct->s_ServerIdNow), "levelpack");
        string davepack = s_NetClient->s_NetInfoStruct->s_ServerList->getValue("Server" + WorkFunction::ConvertFunction::itos(s_NetClient->s_NetInfoStruct->s_ServerIdNow), "davepack");
        s_NetClient->s_GameClass->s_IniFile->setValue("resources", "pooling", "true");
        s_NetClient->s_GameClass->s_IniFile->setValue("resources", "standard", "false");
        if(modpack != "") s_NetClient->s_GameClass->s_IniFile->setValue("resources", "modpack", modpack);
        if(texturepack != "") s_NetClient->s_GameClass->s_IniFile->setValue("resources", "texturepack", texturepack);
        if(monsterpack != "") s_NetClient->s_GameClass->s_IniFile->setValue("resources", "monsterpack", monsterpack);
        if(bonuspack != "") s_NetClient->s_GameClass->s_IniFile->setValue("resources", "bonuspack", bonuspack);
        if(screenpack != "") s_NetClient->s_GameClass->s_IniFile->setValue("resources", "screenpack", screenpack);
        if(soundpack != "") s_NetClient->s_GameClass->s_IniFile->setValue("resources", "soundpack", soundpack);
        if(levelpack != "") s_NetClient->s_GameClass->s_IniFile->setValue("resources", "levelpack", levelpack);
        if(davepack != "") s_NetClient->s_GameClass->s_IniFile->setValue("resources", "davepack", davepack);
        //...
        s_NetClient->s_NetInfoStruct->s_SuperSleep_1 = false;
        s_NetClient->s_NetInfoStruct->s_Sleep_4 = false;
        return;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == "ListCreatures")
    {
        s_NetClient->s_GameClass->setObjects(pps);
        int MyID = atoi( pps->getValue("SystemInfo", "MyID").c_str() );
        s_NetClient->s_GameClass->s_GameInfo->s_MyDave->mergerDave(s_NetClient->s_GameClass->s_GameInfo->s_Daves[MyID]);
        s_NetClient->s_GameClass->removeDave(MyID);
        s_NetClient->s_NetInfoStruct->s_WaitingGettingCreatureList = false;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == "ConfirmKeysStateGetting")
    {
        s_NetClient->s_NetInfoStruct->s_WaitingConfirmGettingKeysState = false;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == "ConfirmLeaveServer")
    {
        s_NetClient->s_NetInfoStruct->s_WaitingConfirmLeaveServer = false;
    }
    else if(pps->getValue("SystemInfo", "ID_MESSAGE") == "Command")
    {
        doCommand(this, pps->getValue("command", "do"), pps);
    }
    delete pps;
}

void Client::doCommand(Client* cl, string command, PostParsingStruct* pps)
{
    //...
}

