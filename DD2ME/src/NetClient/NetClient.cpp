#include "NetClient.h"
#include <iostream>
#include <sstream>
#include <cstdlib>

#include "../Game.h"
#include "../WorkFunctions.h"
#include "../IniParser/ParserInfoFile.h"
#include "../IniParser/PostParsingStruct.h"

#include "../Defines.h"

using namespace std;

using namespace irr;
using namespace net;

using namespace WorkFunctions;
using namespace ParserFunctions;
using namespace STRING_CONSTANTS;

using namespace IniParser;

NetClient::NetClient(Game* gameclass):
    s_GameClass(gameclass),
    s_NetInfo(0),
    s_MyID(-1),
    s_NetManager(0)
{
    s_NetClientCallback = new NetClientCallback(this);
    s_NetInfoStruct = new NetInfoStruct;
    s_NetInfoStruct->s_Error = "";
    s_NetInfoStruct->s_ServerList = 0;
    s_NetInfoStruct->s_ServerIdNow = 0;
    s_NetInfoStruct->s_WaitingConfirmGettingFullInfoFromClient = false;
    s_NetInfoStruct->s_WaitingGettingCreatureList = false;
    s_NetInfoStruct->s_WaitingConfirmLeaveServer = false;
}

NetClient::~NetClient()
{
    if(s_NetClientCallback != 0) delete s_NetClientCallback;
    if(s_NetInfoStruct != 0) delete s_NetInfoStruct;
    if(s_NetInfo != 0) delete s_NetInfo;
}

void NetClient::tick()
{
	//...
}

void NetClient::sendOutPacket(SOutPacket& outpacket, const s32 playerId, const u32 channelID)
{
    outpacket.compressPacket();
    outpacket.encryptPacket(STRING_CONSTANTS::SC_CRYPT_KEY_NET.c_str());
	s_NetManager->sendOutPacket(outpacket, playerId, channelID);
}

void NetClient::sendOutPacketUnreliable(SOutPacket& outpacket, const s32 playerId, const u32 channelID, bool isSequenced)
{
    outpacket.compressPacket();
    outpacket.encryptPacket(STRING_CONSTANTS::SC_CRYPT_KEY_NET.c_str());
	s_NetManager->sendOutPacketUnreliable(outpacket, playerId, channelID, !isSequenced);
}

bool NetClient::connect()
{
    try
    {
        SNetParams snp;
        snp.numberChannels = NUMBER_CONSTANTS::NC_ENET_NUMBER_CHANNELS;
        snp.connectionTimeout = atoi( s_NetInfo->getValue("internet", "timeoutconnect").c_str() ) * 1000;
        snp.downBandwidth = atoi( s_NetInfo->getValue("internet", "downbandwidth").c_str() );
        snp.upBandwidth = atoi( s_NetInfo->getValue("internet", "upbandwidth").c_str() );

        NetClientCallback* clientCallback = new NetClientCallback(this);
		s_NetManager = createIrrNetClient(clientCallback, s_NetInfoStruct->s_Host.c_str(), s_NetInfoStruct->s_Port, snp);
		//s_NetManager->setVerbose(true);

		if(s_NetManager->getConnectionStatus() != net::EICS_FAILED) s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Connected.");
		else
        {
            s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Connection error!", true);
            return false;
        }
    }
    catch(std::exception& e)
	{
	    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, e.what(), true);
		return false;
	}
	s_NetInfoStruct->s_Sleep_1 = true;
	s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Waiting confirm...");
	for(int i = 0; s_NetInfoStruct->s_Sleep_1 == true; i++)
    {
        s_NetManager->update(1000);
        cout << ".";
        if(i == 10)
        {
            cout << endl;
            s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Not confirm.", true);
            return false;
        }
    }
    cout << endl;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Confirmed.");
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Authorization...");
    string str_send = "";
	str_send = addMainVariableString(str_send, "login", SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "name", s_NetInfoStruct->s_Name, SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "pass", s_NetInfoStruct->s_Pass, SPLITTER_STR_VARIABLE);
	str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_CLIENT_IDS_MESSAGES::FCIM_PlayerAuth, SPLITTER_STR_VARIABLE);

	SOutPacket packet;
	packet << str_send;
	sendOutPacket(packet);

	s_NetInfoStruct->s_Sleep_2 = true;
	for(int i = 0; s_NetInfoStruct->s_Sleep_2 == true; i++)
    {
        s_NetManager->update(1000);
        cout << ".";
        if(i == 10)
        {
            cout << endl;
            s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Server timeout.", true);
            return false;
        }
    }
    if(s_NetInfoStruct->s_Error != "")
    {
        cout << endl;
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, s_NetInfoStruct->s_Error, true);
        s_NetInfoStruct->s_Error = "";
        return false;
    }
    cout << endl;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "You are logged.");
    return true;
}

bool NetClient::netGameStartWork()
{
    if(!getServerList())
    {
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Error getting server list.", true);
        return false;
    }
    if(!choiceServer())
    {
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Choicing server failed.", true);
        return false;
    }
    return true;
}

bool NetClient::getServerList()
{
    string str_send;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Getting server list...");
    sendCommandToServer(SERVER_COMMANDS_FROM_CLIENT::SCFC_getServerList);
    s_NetInfoStruct->s_Sleep_3 = true;
    for(int i = 0; s_NetInfoStruct->s_Sleep_3 == true; i++)
    {
        s_NetManager->update(1000);
        cout << ".";
        if(i == atoi( s_NetInfo->getValue("internet", "timeoutconnect").c_str() ) )
        {
            cout << endl;
            s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Server timeout.", true);
            return false;
        }
    }
    cout << endl;
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Server list was getted");
    return true;
}

bool NetClient::choiceServer()
{
    int num, number = 0;
    cout<<"Choice server please (number) or type '0' for exit: ";
    cin>>num;
    if(num == 0)
    {
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Exiting...");
        return false;
    }
    map<string, map<string, string > >::iterator iter;
    for(iter = s_NetInfoStruct->s_ServerList->getMapVariables().begin(); iter != s_NetInfoStruct->s_ServerList->getMapVariables().end(); iter++)
    {
        if(iter->first.find("Server") == 0) number++;
        if(number == num) break;
    }
    if(number != num)
    {
        s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Number of server is bad.");
        return false;
    }
    s_NetInfoStruct->s_ServerIdNow = atoi( s_NetInfoStruct->s_ServerList->getValue(iter->first, "id").c_str() );
    sendCommandToServer(SERVER_COMMANDS_FROM_CLIENT::SCFC_connectToServer);
    s_GameClass->s_Logger->registerEvent(EVENT_TYPE_INFO, "Connecting to game server...");
	s_NetInfoStruct->s_SuperSleep_1 = true;
	while(s_NetInfoStruct->s_SuperSleep_1 == true)
    {
        s_NetInfoStruct->s_Sleep_4 = true;
        for(int i = 0; s_NetInfoStruct->s_Sleep_4 == true; i++)
        {
            s_NetManager->update(1000);
            cout << ".";
            if(i == atoi( s_NetInfo->getValue("internet", "timeoutconnect").c_str() ) )
            {
                cout << endl;
                s_GameClass->s_Logger->registerEvent(EVENT_TYPE_ERROR, "Server timeout.", true);
                return false;
            }
        }
    }
    cout << endl;
    getCreaturesList();
	return true;
}

void NetClient::getCreaturesList(bool notfullfornetmode)
{
    string params = "notfullfornetmode";
    if(!notfullfornetmode) params = "";
    sendCommandToServer(SERVER_COMMANDS_FROM_CLIENT::SCFC_getCreaturesList, params);
	s_NetInfoStruct->s_WaitingGettingCreatureList = true;
}

void NetClient::sendFullInfoFromClient()
{
    PostParsingStruct* pps = s_GameClass->s_GameInfo->s_MyPlayer->getListOfVariables("player");
    ParserInfoFile prs;
    string str_send = prs.convertPostParsingStructToString(pps, SPLITTER_STR_VARIABLE);
    str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
    str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_CLIENT_IDS_MESSAGES::FCIM_FullInfoFromClient, SPLITTER_STR_VARIABLE);

	SOutPacket packet;
	packet << str_send;
	sendOutPacket(packet);

	s_NetInfoStruct->s_WaitingConfirmGettingFullInfoFromClient = true;
}

void NetClient::sendInfoFromClient()
{
    SOutPacket packet;

    packet << PT_PLAYER_COORDS << s_GameClass->s_GameInfo->s_MyPlayer->s_CoordX << s_GameClass->s_GameInfo->s_MyPlayer->s_CoordY;
    sendOutPacketUnreliable(packet, -1, 2, true);
    packet.clearData();

    packet << PT_PLAYER_STATE << s_GameClass->s_GameInfo->s_MyPlayer->s_State << s_GameClass->s_GameInfo->s_MyPlayer->s_StateBeforeOpenDoor << s_GameClass->s_GameInfo->s_MyPlayer->s_Cartridges <<
                    s_GameClass->s_GameInfo->s_MyPlayer->s_NumberOfAction << s_GameClass->s_GameInfo->s_MyPlayer->s_AdditionalNumberOfAction << s_GameClass->s_GameInfo->s_MyPlayer->s_OldNumberOfAction << s_GameClass->s_GameInfo->s_MyPlayer->s_ShootNow;
    sendOutPacketUnreliable(packet, -1, 2, true);
    packet.clearData();

    packet << PT_PLAYER_KEYS_STATE << s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyDown << s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyJump << s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyLeft <<
                    s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyLeftDown << s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyLeftUp << s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyRight <<
                    s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyRightDown << s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyRightUp << s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyShoot <<
                    s_GameClass->s_GameInfo->s_MyPlayer->s_KeysState->s_KeyUp << s_GameClass->s_GameInfo->s_MyPlayer->s_ControlJumpPressed << s_GameClass->s_GameInfo->s_MyPlayer->s_ControlShootPressed;
    sendOutPacketUnreliable(packet, -1, 2, true);
    packet.clearData();
}

void NetClient::sendOpenDoor(string type, int x, int y)
{
    SOutPacket packet;
	packet << PT_OPEN_DOOR << type << x << y;
	sendOutPacket(packet, -1, 3);
}

void NetClient::sendSetTileID(int x, int y, int numberfield, int tileid)
{
    SOutPacket packet;
	packet << PT_SET_TILE_ID << x << y << numberfield << tileid;
	sendOutPacket(packet, -1, 3);
}

void NetClient::leaveServer()
{
    sendCommandToServer(SERVER_COMMANDS_FROM_CLIENT::SCFC_leaveServer);
	s_NetInfoStruct->s_WaitingConfirmLeaveServer = true;
}

void NetClient::sendCommandToServer(string command, string params)
{
    string str_send = "";
    str_send = addMainVariableString(str_send, "SystemInfo", SPLITTER_STR_VARIABLE);
    str_send = addSecondaryVariableString(str_send, "ID_MESSAGE", FROM_CLIENT_IDS_MESSAGES::FCIM_Command, SPLITTER_STR_VARIABLE);
    str_send = addMainVariableString(str_send, "command", SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "do", command, SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "id", WorkFunctions::ConvertFunctions::itos(s_NetInfoStruct->s_ServerIdNow), SPLITTER_STR_VARIABLE);
	str_send = addSecondaryVariableString(str_send, "params", params, SPLITTER_STR_VARIABLE);

	SOutPacket packet;
	packet << str_send;
	sendOutPacket(packet);

	s_MyID = -1;
}
