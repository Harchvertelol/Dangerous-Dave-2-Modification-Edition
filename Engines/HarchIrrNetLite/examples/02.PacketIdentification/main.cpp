/*
Example 2: Packet Identification.

In this example I will expain the basics of packet identification.
For a more basic introduction please refer to the tutorial first.
(Recommended)
*/

// Do the usual things here.
#include <irrNet.h>
#include <iostream>

using namespace irr;

#pragma comment(lib, "irrNetLite.lib")
#pragma comment(lib, "ws2_32.lib")

// Now, in this example, I will teach you the basics of packet
// identification. What is packet identification? For instance,
// lets say the client sends 3 floating point values to the server,
// how will the server now if these represent a position, a rotation,
// or something completely different? How will it even know these
// are floating point values and not integers, or maybe a 12-byte
// string? Obviously, we need some kind of mechanism to define this.
// So in this example, we are going to define a few basic packet
// types, that let us differentiate between the different kinds of
// information coming into our client/server.

// I will start off by creating an enum that holds a simple id
// for each of our packet types. (Note, you don't have to
// explicitly set the values like I do, I am just doing that
// for clarity.)
enum E_PACKET_TYPE
{
	EPT_ROTATION = 1,
	EPT_POWER = 2,
	EPT_MESSAGE = 3
};

// In this example we will create 2 different callback classes, one
// for the clients and one for the server. We will have an imaginary
// cannon on the server, and the clients can send messages to change
// it's angle and power.

// The server callback.
class ServerNetCallback : public net::INetCallback
{
	// We will store a pointer to the net manager.
	net::INetManager* netManager;

	// Here we will store the cannon's power and angle.
	// Remember that f32 is just a typedef for float.
	f32 cannonPower;
	f32 cannonAngle;
public:
	ServerNetCallback(net::INetManager* netManagerIn) : netManager(netManagerIn) {}


	// Our handlePacket function.
	virtual void handlePacket(net::SInPacket& packet, u32 channelID)
	{
		// The packets will use a single char to store
		// the packet identifier, remember to use the
		// smallest possible datatype for storing your
		// packet identifiers. c8 is a typedef for char.
		c8 packetid;
		packet >> packetid;

		// Here we will switch based on the packet id.
		switch((E_PACKET_TYPE)packetid)
		{
		case EPT_ROTATION:
			// We obtain the cannon angle from the packet.
			packet >> cannonAngle;
			std::cout << "The cannon angle is now " << cannonAngle << std::endl;
			break;
		case EPT_POWER:
			// Same here.
			packet >> cannonPower;
			std::cout << "The cannon power is now " << cannonPower << std::endl;
			break;
		case EPT_MESSAGE:
			// Now this is a special case (Pun intended, or is it even a pun?... nevermind),
			// The client has sent us a message as a string, so we will just print that
			// to the console.
			core::stringc message;
			packet >> message;

			// We can obtain a unique "player id" from a packet identifying the client that
			// sent it. More details about the player id in a later example, for now we will
			// just print it out with the message so we know whos saying what.
			std::cout << "Client " << packet.getPlayerId() << " says: " << message.c_str();
			std::cout << std::endl;
			break;
		}

		// After handling a packet, we will send an updated status of the cannon to all clients.
		net::SOutPacket rotationPacket;

		// The packet id is the first thing that goes in a packet. Note that I am casting it to a char,
		// because that is what we want to store it as, to save space. Be careful to use the same types
		// when sending and receiving, don't send as a char and receive as an int, it will cause trouble.
		rotationPacket << (c8)EPT_ROTATION;
		rotationPacket << cannonAngle;

		// Send the packet to all connected clients.
		netManager->sendOutPacket(rotationPacket);

		// Send a power update too.
		net::SOutPacket powerPacket;
		powerPacket << (c8)EPT_POWER;
		powerPacket << cannonPower;
		netManager->sendOutPacket(powerPacket);
	}
};

// The client callback.
class ClientNetCallback : public net::INetCallback
{
public:
	// Our handlePacket function.
	virtual void handlePacket(net::SInPacket& packet, u32 channelID)
	{
		// Just like the server, we obtain the packet id and print
		// the information based on the packet we received. I hope the
		// rest of this function is self-explanatory.
		c8 packetid;
		packet >> packetid;

		switch((E_PACKET_TYPE)packetid)
		{
		case EPT_ROTATION:
			f32 cannonAngle;
			packet >> cannonAngle;
			std::cout << "Server says that the cannon angle is now " << cannonAngle << std::endl;
			break;
		case EPT_POWER:
			f32 cannonPower;
			packet >> cannonPower;
			std::cout << "Server says that the cannon power is now " << cannonPower << std::endl;
			break;
		default:
			// We don't care about any other types, so we catch them here and break.
			break;
		}
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
		// Create an irrNetLite server. We won't specifiy a listen port,
		// so the default port of 45000 will be used.
		net::INetManager* netManager = net::createIrrNetServer(0);

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
		// Create a client and pass in the client callback. Since the server is
		// using the default listen port of 45000, we don't need to pass it in.
		// You may want to change the ip address to a remote one and experiment
		// with connecting to a remote host.
		ClientNetCallback* clientCallback = new ClientNetCallback();
		net::INetManager* netManager = net::createIrrNetClient(clientCallback, "127.0.0.1");

		if (netManager->getConnectionStatus() == net::EICS_FAILED)
			return 0;

		// Print a simple menu.
		std::cout 	<< "You are connected! What would you like to do?" << std::endl
				<< "1. Change the cannon rotation." << std::endl
				<< "2. Change the cannon power." << std::endl
				<< "3. Send a message." << std::endl;

		// Take the input.
		char i = 0;
		std::cin >> i;

		// Switch based on input.
		switch(i)
		{
			case '1':
			{
				// Here we create a rotation packet and send it to the server.
				net::SOutPacket rotationPacket;
				rotationPacket << (c8)EPT_ROTATION; // Remember to cast to the correct type.

				// Ask for the rotation.
				f32 rotation;
				std::cout << "Please enter a rotation: ";
				std::cin >> rotation;
				rotationPacket << rotation;
				netManager->sendOutPacket(rotationPacket);
				break;
			}
			case '2':
			{
				// And here we create a power packet and send it to the server.
				net::SOutPacket powerPacket;
				powerPacket << (c8)EPT_POWER; // Remember to cast to the correct type.
				// Ask for the power.
				f32 power;
				std::cout << "Please enter the power: ";
				std::cin >> power;
				powerPacket << power;
				netManager->sendOutPacket(powerPacket);
				break;
			}
			case '3':
			{
				// We'll also send the server a message in the form of a string.
				net::SOutPacket messagePacket;
				messagePacket << (c8)EPT_MESSAGE; // Remember to cast to the correct type.
				// Ask for the message.
				char message[512] = {};
				std::cout << "Please enter a message:" << std::endl;
				std::cin >> message;
				messagePacket << message;
				netManager->sendOutPacket(messagePacket);
				break;
			}
			default:
				break;
		}

		// Here is the update loop, we will exit if there is a connection problem.
		while(netManager->getConnectionStatus() != net::EICS_FAILED)
		{
			// Here we update.
			netManager->update(1000);
		}

		// Clean up.
		delete netManager;
		delete clientCallback;
	}

	// And we're done, return 0 and make like a banana.
	return 0;
}
