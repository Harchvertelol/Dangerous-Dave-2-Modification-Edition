#include <iostream>

#include "NetServerCallback.h"
#include "Server.h"
#include "../UserData.h"
#include "../Defines.h"
#include "../MainServer.h"

#include "../NetClient/NetInfoStruct.h"

using namespace std;

using namespace irr;
using namespace net;

NetServerCallback::NetServerCallback(Server* s):
    s_Server(s)
{
    //...
}

NetServerCallback::~NetServerCallback()
{
    //...
}

void NetServerCallback::handlePacket(SInPacket& packet, u32 channelID)
{
    packet.decryptPacket(STRING_CONSTANTS::SC_CRYPT_KEY_NET.c_str());
    packet.deCompressPacket();

    if(channelID == 0)
    {
        string str;
        packet >> str;
        s_Server->onCommand(packet.getPlayerId(), str);
    }
    else if(channelID == 2)
    {
        int packet_type;
        packet >> packet_type;
        int playerId = packet.getPlayerId();
        if(packet_type == PT_PLAYER_COORDS)
        {
            int new_x, new_y;
            packet >> new_x;
            packet >> new_y;
            if(s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players.find(playerId) != s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players.end())
            {
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_CoordX = new_x;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_CoordY = new_y;
            }

            SOutPacket packet;
            packet << PT_PLAYER_COORDS << playerId << new_x << new_y;
            s_Server->sendOutPacketUnreliable(packet, -1, 2, true);
        }
        else if(packet_type == PT_PLAYER_STATE)
        {
            string state, statebod;
            int cartridges, numberoa, addnumberoa, oldnumberoa, shootnow;
            packet >> state;
            packet >> statebod;
            packet >> cartridges;
            packet >> numberoa;
            packet >> addnumberoa;
            packet >> oldnumberoa;
            packet >> shootnow;
            if(s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players.find(playerId) != s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players.end())
            {
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_State = state;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_StateBeforeOpenDoor = statebod;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_Cartridges = cartridges;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_NumberOfAction = numberoa;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_AdditionalNumberOfAction = addnumberoa;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_OldNumberOfAction = oldnumberoa;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_ShootNow = shootnow;

            }

            SOutPacket packet;
            packet << PT_PLAYER_STATE << playerId << state << statebod << cartridges << numberoa << addnumberoa << oldnumberoa << shootnow;
            s_Server->sendOutPacketUnreliable(packet, -1, 2, true);
        }
    }
}

void NetServerCallback::onDisconnect(const u16 playerId)
{
	std::cout << "Socket closed" << endl;
	s_Server->close(playerId);
}

void NetServerCallback::onConnect(const u16 playerId)
{
	cout << "Accepted new connection!" << endl;
	s_Server->accept(playerId);
}

