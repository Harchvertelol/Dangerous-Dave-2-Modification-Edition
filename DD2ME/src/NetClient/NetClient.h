#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <string>

#include <irrNet.h>

#include "NetClientCallback.h"
#include "NetInfoStruct.h"
#include "../IniParser/PostParsingStruct.h"

class Game;

class NetClient
{
    public:
        NetClient(Game*);
        ~NetClient();
        NetClientCallback* s_NetClientCallback;
        NetInfoStruct* s_NetInfoStruct;
        Game* s_GameClass;
        IniParser::PostParsingStruct* s_NetInfo;
        int s_MyID;
        bool connect();
        bool netGameStartWork();
        bool getServerList();
        void getCreaturesList(bool notfullfornetmode = false);
        bool choiceServer();
        void sendFullInfoFromClient();
        void sendInfoFromClient();
        void sendOpenDoor(std::string type, int x, int y);
        void sendSetTileID(int x, int y, int numberfield, int tileid);
        void leaveServer();
        void sendCommandToServer(std::string, std::string params = "");
        void sendOutPacket(irr::net::SOutPacket& outpacket, const irr::s32 playerId = -1, const irr::u32 channelID = 0);
        void sendOutPacketUnreliable(irr::net::SOutPacket& outpacket, const irr::s32 playerId = -1, const irr::u32 channelID = 0, bool isSequenced = false);
        irr::net::INetManager* s_NetManager;
    private:
        void tick();
};

#endif

