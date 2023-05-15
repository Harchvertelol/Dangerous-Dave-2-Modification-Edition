/*
Example 3: Client management.

In this example I will demonstrate client management.
For a more basic introduction please refer to the Tutorial first.
(Recommended)
*/

// Do the usual things here.
#include <irrNet.h>
#include <iostream>

using namespace irr;

#pragma comment(lib, "irrNetLite.lib")
#pragma comment(lib, "ws2_32.lib")

// In this tutorial, I will show you how to kick/ban clients,
// set a limit on how many clients can join a server, and
// respond when a client connects/disconnects.

// Our server callback will be doing a little more than just
// handling packets this time. We will override 2 more virtual
// methods, onConnect and onDisconnect, that will notify us
// with the player id of the client that has just connected or
// disconnected. What is a player id? A player id is a unique
// number that irrNetLite assigns to a client when they connect
// to the server. The ids will start at 1 and grow until there
// are no available slots left. Ids are not specific to a
// computer or ip address, and are re-used when freed,
// so if the client with played id 1 disconnects, and
// a different one connects, he will take over that player id.
// You may be thinking, "But how do I know which player is
// connecting?" or, "How can I ban a player with a specific
// ip address using this system?". These questions will be
// answered in this tutorial, so pay close attention.
class ServerNetCallback : public net::INetCallback
{
public:
	ServerNetCallback(net::INetManager* netManagerIn) : netManager(netManagerIn) {}

	// Override the "onConnect" function, don't forget to get the method
	// signature correct. Whenever a fresh client connects, their brand
	// spanking new player id will be passed to this function. A "u16"
	// is a typedef for an unsigned short, incase you were wondering.
	virtual void onConnect(const u16 playerId)
	{
		// When a client connects we inform all other connected
		// clients that a client with that player id has connected.

		// But first, lets say that we have already banned a few
		// clients from this server, and that we are keeping a list
		// of all the banned ip addresses. We can simply use
		// "getClientAddress()", to obtain the 32-bit representation
		// of the ip address and check it against the list.
		bool playerIsBanned = false;
		for(int i = 0;i < banList.size();++i)
		{
			if(netManager->getClientAddress(playerId) == banList[i])
			{
				playerIsBanned = true;
				break;
			}
		}

		// If the player is banned, send a message to all connected
		// clients that a player that is banned tried to connect
		// and then kick the naughty player.
		if(playerIsBanned)
		{
			net::SOutPacket packet;
			packet << 	"A player that is banned tried to connect, " \
				 	"so I kicked them.";
			netManager->sendOutPacket(packet);

			// Kick the client by passing the player id.
			netManager->kickClient(playerId);
		}
		else // Else we tell everyone who connected.
		{
			// I am using a core::stringc here, it is very
			// similar to a std::string. You can use a std::string
			// if you want.
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

	// Similar to the onConnect function, except it happens when a
	// player disconnects. When this happens we will just report
	// which player has disconnected.
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

	// Handle the packets, as usual.
	virtual void handlePacket(net::SInPacket& packet, u32 channelID)
	{
		// Now, we need a good reason to ban players.
		// Lets say I don't like people who talk alot,
		// so lets ban anyone who sends a greeting message
		// that is longer than 20 characters, else just
		// print the welcome message.
		core::stringc message;
		packet >> message;

		// We can grab the unique player id of the player
		// that sent the packet from the packet itself.
		u16 playerId = packet.getPlayerId();


		if(message.size() > 20)
		{
			// Kick and ban the player by adding their address to our list.
			netManager->kickClient(playerId);
			u32 address = netManager->getClientAddress(playerId);
			banList.push_back(address);
			std::cout << "Player from " << address << " banned." << std::endl;
		}
		else
		{
			// Print the message.
			std::cout << "Client " << playerId << " said " << message.c_str() << std::endl;
		}
	}

private:
	// An array of "u32", a typedef for unsigned int. This core::array is a
	// custom implementation of a dynamic array, very similar to std::vector.
	core::array<u32> banList;

	// A pointer to the INetManager.
	net::INetManager* netManager;
};

// The client callback.
class ClientNetCallback : public net::INetCallback
{
public:
	// Our handlePacket function.
	virtual void handlePacket(net::SInPacket& packet, u32 channelID)
	{
		// Very simple callback, just echo what the server says.
		core::stringc message;
		packet >> message;
		std::cout << "Server says: " << message.c_str() << std::endl;
	}
};

int main()
{
	// Ask the user whether they want to be the server or a client.
	std::cout << "Client (c) or Server (s)?";
	char i;
	std::cin >> i;

	// If they typed 's' they are the server else they are the client.
	if(i == 's')
	{
		// Create an irrNetLite server. In this example we decide to listen on
		// port 65535, the highest port allowed.
		net::INetManager* netManager = net::createIrrNetServer(0,65535);

		// Pass in a server specific net callback.
		ServerNetCallback* serverCallback = new ServerNetCallback(netManager);
		netManager->setNetCallback(serverCallback);

		// Here we update like usual, most of the logic is in the callback.
		while(netManager->getConnectionStatus() != net::EICS_FAILED)
			netManager->update(1000);

		// Delete everything.
		delete netManager;
		delete serverCallback;
	}
	else
	{
		// Create a client and pass in the client callback.
		// You may want to change the ip address to a remote one and experiment
		// with connecting to a remote host, especially for this example as,
		// if you run all the clients from the same pc and ban one, you
		// won't be able to create anymore clients unless you restart the server.
		ClientNetCallback* clientCallback = new ClientNetCallback();
		net::INetManager* netManager = net::createIrrNetClient(clientCallback, "127.0.0.1", 65535);

		// The clients in this example will simply send a custom greeting message
		// when they connect and then wait and poll for events.

		// If there wasn't a problem connecting we will send a greeting message.
		if(netManager->getConnectionStatus() != net::EICS_FAILED)
		{
			// Print a simple menu.
			std::cout << "You are connected! Please enter a greeting message:" << std::endl;

			// Take the input.
			std::string message;
			std::cin >> message;

			// Send a packet with the message entered.
			net::SOutPacket packet;
			packet << message;
			netManager->sendOutPacket(packet);
		}

		// Here is the update loop, we will exit if there is a connection problem
		// or after running for 10 seconds.
		int i = 0;
		while(netManager->getConnectionStatus() != net::EICS_FAILED && i < 10)
		{
			// Here we update.
			netManager->update(1000);
			++i;
		}

		// Clean up.
		delete netManager;
		delete clientCallback;
	}

	// And we're done, return 0 and make like an egg.
	return 0;
}
