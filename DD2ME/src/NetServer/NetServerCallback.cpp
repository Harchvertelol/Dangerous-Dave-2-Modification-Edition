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

                SOutPacket packetOut;
                packetOut << PT_PLAYER_COORDS << playerId << new_x << new_y;
                s_Server->sendOutPacketUnreliable(packetOut, -1, 2, true);
            }
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
                //s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_NumberOfAction = numberoa;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_AdditionalNumberOfAction = addnumberoa;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_OldNumberOfAction = oldnumberoa;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_ShootNow = shootnow;

                SOutPacket packetOut;
                packetOut << PT_PLAYER_STATE << playerId << state << statebod << cartridges << numberoa << addnumberoa << oldnumberoa << shootnow;
                s_Server->sendOutPacketUnreliable(packetOut, -1, 2, true);
            }
        }
        else if(packet_type == PT_PLAYER_KEYS_STATE)
        {
            if(s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players.find(playerId) != s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players.end())
            {
                int key_down, key_jump, key_left, key_leftdown, key_leftup, key_right, key_rightdown, key_rightup, key_shoot, key_up, ctrl_jmp_prs, ctrl_sht_prs;

                packet >> key_down;
                packet >> key_jump;
                packet >> key_left;
                packet >> key_leftdown;
                packet >> key_leftup;
                packet >> key_right;
                packet >> key_rightdown;
                packet >> key_rightup;
                packet >> key_shoot;
                packet >> key_up;
                packet >> ctrl_jmp_prs;
                packet >> ctrl_sht_prs;

                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_KeysState->s_KeyDown = (bool)key_down;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_KeysState->s_KeyJump = (bool)key_jump;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_KeysState->s_KeyLeft = (bool)key_left;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_KeysState->s_KeyLeftDown = (bool)key_leftdown;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_KeysState->s_KeyLeftUp = (bool)key_leftup;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_KeysState->s_KeyRight = (bool)key_right;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_KeysState->s_KeyRightDown = (bool)key_rightdown;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_KeysState->s_KeyRightUp = (bool)key_rightup;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_KeysState->s_KeyShoot = (bool)key_shoot;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_KeysState->s_KeyUp = (bool)key_up;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_ControlJumpPressed = (bool)ctrl_jmp_prs;
                s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_GameInfo->s_Players[playerId]->s_ControlShootPressed = (bool)ctrl_sht_prs;

                SOutPacket packetOut;
                packetOut << PT_PLAYER_KEYS_STATE << playerId << key_down << key_jump << key_left << key_leftdown << key_leftup << key_right << key_rightdown << key_rightup << key_shoot << key_up << ctrl_jmp_prs << ctrl_sht_prs;
                s_Server->sendOutPacketUnreliable(packetOut, -1, 2, true);
            }
        }
    }
    else if(channelID == 3)
    {
        int packet_type;
        packet >> packet_type;
        int playerId = packet.getPlayerId();
        if(packet_type == PT_OPEN_DOOR)
        {
            string type;
            int x, y;
            packet >> type;
            packet >> x;
            packet >> y;
            s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_Data->s_Level->openDoor(type, x, y);
        }
        else if(packet_type == PT_SET_TILE_ID)
        {
            int x, y, numberfield, tileid;
            packet >> x;
            packet >> y;
            packet >> numberfield;
            packet >> tileid;
            s_Server->s_MainServer->s_ListGameClass[s_Server->s_Clients[playerId].s_IdServerConnected]->s_Data->s_Level->setTileID(x, y, numberfield, tileid);
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

