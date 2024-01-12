#ifndef SERVER_H
#define SERVER_H

#include <set>
#include <map>

#include <irrNet.h>

#include "NetServerCallback.h"
#include "../IniParser/PostParsingStruct.h"
#include "../UserData.h"

class MainServer;

class Server
{
    public:
        Server(MainServer*);
        ~Server();
        MainServer* s_MainServer;
        irr::net::INetManager* s_NetManager;
        std::map<std::string, int> s_ClientsId;
        std::map<int, UserData> s_Clients;
        void accept(const int);
        void run(int port);
        void close(const int);
        void onCommand(const int, const std::string&);
        void doCommand(const int, std::string, IniParser::PostParsingStruct*);
        void sendOutPacket(irr::net::SOutPacket& outpacket, const irr::s32 playerId = -1, const irr::u32 channelID = 0);
        void sendOutPacketUnreliable(irr::net::SOutPacket& outpacket, const irr::s32 playerId = -1, const irr::u32 channelID = 0, bool isSequenced = false);
        void tick();
        //...
        void sendCreaturesList(std::string gameclassid, std::string params, bool notfullfornetmode = false, int playerId = 0, bool IsUnreliable = false);
        void playerLeaveServer(int playerId, bool sendconfirm = true);
        void sendOpenDoor(std::string type, int x, int y);
        void sendSetTileID(int x, int y, int numberfield, int tileid);
};

#endif // SERVER_H

