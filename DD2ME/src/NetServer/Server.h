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
        void accept(const int);
        void run(int port);
        void close(const int);
        void onCommand(const int, const std::string&);
        void doCommand(const int, std::string, IniParser::PostParsingStruct*);
        void sendOutPacket(irr::net::SOutPacket& outpacket, const irr::s32 playerId = -1, const irr::u32 channelID = 0);
        void sendOutPacketUnreliable(irr::net::SOutPacket& outpacket, const irr::s32 playerId = -1, const irr::u32 channelID = 0, bool isUnsequenced = false);
        irr::net::INetManager* s_NetManager;
    private:
        void tick();
        std::map<std::string, int> s_ClientsId;
        std::map<int, UserData> s_Clients;
};

#endif // SERVER_H

