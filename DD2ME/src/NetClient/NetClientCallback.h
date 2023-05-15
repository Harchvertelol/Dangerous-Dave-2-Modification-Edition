#ifndef CLIENTCALLBACK_H
#define CLIENTCALLBACK_H

#include <string>

#include <irrNet.h>

#include "../IniParser/PostParsingStruct.h"

class NetClient;

class NetClientCallback: public irr::net::INetCallback
{
    public:
        NetClientCallback(NetClient*);
        ~NetClientCallback();
        NetClient* s_NetClient;
        void workStr(std::string);
        void doCommand(NetClientCallback*, std::string, IniParser::PostParsingStruct*);
        void connect(const std::string&, int port);
    private:
        void handlePacket(irr::net::SInPacket& packet, irr::u32 channelID);
        void onConnect(const irr::u16 playerId);
        void onDisconnect(const irr::u16 playerId);
};

#endif // CLIENTCALLBACK_H


