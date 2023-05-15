#ifndef NETSERVERCALLBACK_H
#define NETSERVERCALLBACK_H

#include <irrNet.h>

class Server;

class NetServerCallback: public irr::net::INetCallback
{
    public:
        NetServerCallback(Server*);
        ~NetServerCallback();
        Server* s_Server;
    private:
        void handlePacket(irr::net::SInPacket& packet, irr::u32 channelID);
        void onConnect(const irr::u16 playerId);
        void onDisconnect(const irr::u16 playerId);
};

#endif // NETSERVERCALLBACK_H
