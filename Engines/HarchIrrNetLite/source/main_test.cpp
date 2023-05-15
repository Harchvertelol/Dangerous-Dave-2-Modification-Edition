#include <irrNet.h>
#include <iostream>
#include <conio.h>

using namespace irr;

class ServerNetCallback : public net::INetCallback
{
public:
	ServerNetCallback(net::INetManager* netManagerIn) : netManager(netManagerIn), numbrem(0) {}

	virtual void onConnect(const u16 playerId)
	{
		bool playerIsBanned = false;
		for(int i = 0;i < banList.size();++i)
		{
			if(netManager->getClientAddress(playerId) == banList[i])
			{
				playerIsBanned = true;
				break;
			}
		}
		if(playerIsBanned)
		{
			net::SOutPacket packet;
			packet << 	"A player that is banned tried to connect, " \
				 	"so I kicked them.";
			netManager->sendOutPacket(packet);
			netManager->kickClient(playerId);
		}
		else
		{
			net::SOutPacket packet;
			core::stringc message;
			message = "Client number ";
			message += playerId;
			message += " has just connected. ";
			message += netManager->getPeerCount();
			message += " peer(s) total.";
			packet << message;
			netManager->sendOutPacket(packet);

			std::cout << "Client number " << playerId << " connected. "
			<< netManager->getPeerCount() << " peer(s) total." << std::endl;

		}
	}

	virtual void onDisconnect(const u16 playerId)
	{
		net::SOutPacket packet;
		core::stringc message;
		message = "Client number ";
		message += playerId;
		message += " has just left the building. ";
		message += netManager->getPeerCount();
		message += " peer(s) left.";
		packet << message;
		netManager->sendOutPacket(packet);

		std::cout << "Client number " << playerId << " disconnected. "
			<< netManager->getPeerCount() << " peer(s) left." << std::endl;

	}

	virtual void handlePacket(net::SInPacket& packet, u32 channelID)
	{
		int message;
		packet >> message;

		u16 playerId = packet.getPlayerId();


		/*if(message.size() > 20)
		{
		    net::SOutPacket packet;
			packet << 	"Message too long, " \
				 	"so I kicked and banned them.";
			netManager->sendOutPacket(packet);
			netManager->kickClient(playerId);
			u32 address = netManager->getClientAddress(playerId);
			banList.push_back(address);
			std::cout << "Player from " << address << " banned." << std::endl;
		}
		else*/
		{
		    u32 pingclient = netManager->getPing(packet.getPlayerId());
			std::cout << "Client " << playerId << " said " << message << ", ping: " << pingclient << ", channelID: " << channelID << std::endl;
			if(numbrem + 1 != message)
            {
                std::cout << "ERROR! LOST " << numbrem + 1 << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                if(message < numbrem)
                {
                    std::cout << "SEQUENCED!!!!!!!!!!!!!@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
                    netManager->kickClient(playerId);
                }
            }
			numbrem = message;
		}
	}

private:
	core::array<u32> banList;
	net::INetManager* netManager;
	int numbrem;
};

class ClientNetCallback : public net::INetCallback
{
public:
    ClientNetCallback(): incc(0){}
    net::INetManager* incc;
	virtual void handlePacket(net::SInPacket& packet, u32 channelID)
	{
		core::stringc message;
		packet >> message;
		std::cout << "Server says: " << message.c_str() << "Channel ID: " << channelID << std::endl;
	}
};

int main()
{
	std::cout << "Client (c) or Server (s)?";
	char i;
	std::cin >> i;

	irr::net::SNetParams snp;
	snp.numberChannels = 2;

	if(i == 's')
	{
		net::INetManager* netManager = net::createIrrNetServer(0, 65535, snp);

		netManager->setVerbose(true);

		ServerNetCallback* serverCallback = new ServerNetCallback(netManager);
		netManager->setNetCallback(serverCallback);

		while(netManager->getConnectionStatus() != net::EICS_FAILED)
        {
            netManager->update(1000);
            //std::cout << "Ping: " << netManager->getPing(1) << std::endl;
        }

		delete netManager;
		delete serverCallback;
	}
	else
	{
		ClientNetCallback* clientCallback = new ClientNetCallback();
		net::INetManager* netManager = net::createIrrNetClient(clientCallback, "127.0.0.1", 65535, snp);
		clientCallback->incc = netManager;

		netManager->setVerbose(true);

		if(netManager->getConnectionStatus() != net::EICS_FAILED)
		{
			std::cout << "You are connected! Please enter a greeting message:" << std::endl;

			int message;
			std::cin >> message;

			net::SOutPacket packet;
			packet << message;
			netManager->sendOutPacket(packet);
		}

		int i = 0;
		while(netManager->getConnectionStatus() != net::EICS_FAILED /*&& i < 10*/)
		{
		    if(kbhit())
            {
                int q;
                std::cin >> q;
            }
			netManager->update(1);
			net::SOutPacket packet;
			packet << i << "sdklfjdslkjfskdljfklsdjfkldsjflkjsdlkfjklsdjfjsdlk";
			netManager->sendOutPacketUnreliable(packet, -1, 2, true);
			++i;
			//std::cout << "ping: " << netManager->getPing() << std::endl;
		}

		delete netManager;
		delete clientCallback;
	}
	return 0;
}
