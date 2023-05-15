#include <iostream>

#include "NetServerCallback.h"
#include "Server.h"
#include "../UserData.h"
#include "../Defines.h"

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

