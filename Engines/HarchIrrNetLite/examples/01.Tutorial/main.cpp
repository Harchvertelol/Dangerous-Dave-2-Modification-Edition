/*
Example 1: Tutorial.

Welcome to the simple, introductory tutorial for irrNetLite. This will show you the
very basics of setting up a client and server, and sending and receiving packets.
Because irrNetLite is so easy to use, I think you will not find many actual lines of
code in this example, rather most of it is documentation.
*/

// Include the main irrNet.h header for irrNetLite.
#include <irrNet.h>
#include <iostream>

// irrNetLite resides within the irr::net:: namespace, therefore,
// we use "using namespace irr" here to simplify things.
// Alternatively you can add "using namespace net" after
// this so that net:: doesn't have to be used either.
using namespace irr;

// These pragmas are for MSVC users, they ease the linking of librarys.
// "ws2_32.lib" is part of the Windows Platform SDK and must be linked
// in when compiling on windows. On linux/mac machines, these pragmas
// will simply be ignored, and the linking to irrNetLite static lib
// should be performed by a makefile.
#pragma comment(lib, "irrNetLite.lib")
#pragma comment(lib, "ws2_32.lib")

// You must derive a class from INetCallback and override the
// "handlePacket" method. When a packet is received it will be
// passed to this function, and you may dissect it as you wish.
// If this is the server you may retrieve the player ID using
// packet.getPlayerId().
class MyNetCallback : public net::INetCallback
{
public:
	virtual void handlePacket(net::SInPacket& packet, u32 channelID)
	{
		// irrNetLite encryption is very easy to use! Just pass
		// a 16-byte (128-bit) string to encryptPacket/decryptPacket
		// to encrypt/decrypt a packet respectively. Do not try to
		// decrypt an un-encrypted packet or read from an encrypted
		// packet without decrypting it first or bad things will happen!
		packet.decryptPacket("hushthisissecret");

		// irrNetLite compression is even easier! The ZLib library is used
		// here, just call compressPacket/decompressPacket to
		// compress/decompress a packet. Again, do not try to decompress
		// an un-compressed packet or read from a compressed packet without
		// decompressing it! Another thing to keep in mind is that you should
		// decompress and decrypt in the correct order. If you compressed a file
		// and then encrypted it when it was sent, you must decrypt it first
		// before trying to decompress it, same goes for the other order.
		packet.deCompressPacket();

		// Extracting info from a received packet is simple. You can treat
		// the packet as an input stream, the >> operator is overloaded
		// and works for most built in types. The string class in irrNetLite
		// is a custom implementation not unlike the std::string. You can
		// also send and receive strings as "char*". Note that the "char*"
		// and stringc are interchangeable, you can send a stringc and
		// receive a char*, or vice-versa.
		core::stringc str;
		packet >> str;

		// Support for a simple 3-dimensional vector class is there too. Both
		// vector3df and core::stringc are borrowed from Irrlicht and included
		// here for convenience.
		core::vector3df vec;
		packet >> vec;

		// Here we are obtaining the last value from the packet. f32 is just a
		// typedef for float.
		f32 height;
		packet >> height;

		// Print the values to the console.
		std::cout << "Message: " << str.c_str();
		std::cout << " Position: " << vec.X << " " << vec.Y << " " << vec.Z;
		std::cout << " Height: " << height << " ft";
		std::cout << std::endl;
	}
};

// Ok, lets go from the start, int main()
int main()
{
	// Ask the user whether they want to be the server or a client.
	std::cout << "Client (c) or Server (s)?";
	char i;
	std::cin >> i;

	// If they typed 's' they are the server else they are the client.
	if(i == 's')
	{
		// Create a server and pass in a new instance of our callback class. The default
		// port that clients can connect to is set to 45000, we'll set it to 60000.
		// Recall that the valid port range is 1-65535.
		MyNetCallback* netCallback = new MyNetCallback();
		net::INetManager* netManager = net::createIrrNetServer(netCallback, 60000);

		// Setting verbose to true makes irrNetLite spit out debug information to the console.
		netManager->setVerbose(true);

		// While the connection is active (Not failed), we update the netManager.
		// Note that since this is a server the connection will pretty much always
		// be flagged as active, unless some error occured whilst creating the server.
		// A value of 1000 is passed to update to make it hang for a second and wait for
		// packets to arrive. (Recommended for servers, so you don't busy-loop).
		while(netManager->getConnectionStatus() != net::EICS_FAILED)
			netManager->update(1000);

		// Don't forget to clean up!
		delete netManager;
		delete netCallback;
	}
	else
	{
		// Create an irrNet client, in this example we will just connect to the localhost
		// address ("127.0.0.1"), which basically means we are connecting to the same
		// computer the client is on. Note that we just pass a value of 0 as our
		// INetCallback, because the client in this example does no need to handle any
		// packets. You can safely pass a value of 0 if this is the case.
		net::INetManager* netManager = net::createIrrNetClient(0, "127.0.0.1", 60000);

		// Enable debug messages.
		netManager->setVerbose(true);


		// Here comes the fun part, while the client is connected we update the netManager
		// and ask it to wait 1 second (1000 milliseconds) for new packets to arrive before
		// returning. Since the client in this example doesn't actually receive any packets,
		// the only purpose of the update call is to leave a 1 second interval between each
		// packet we send. To prevent an infinite loop of messages we will quit after 10 messages.
		int i = 0;
		while(netManager->getConnectionStatus() != net::EICS_FAILED && i < 10 )
		{

			// To send a packet, first you create an SOutPacket object.
			net::SOutPacket packet;

			// Then you can use the streaming operator << to add new data to it.
			packet << "Help I am stuck on a mountain!";

			// You can even chain the << operators like so, just like with ostream.
			packet << core::vector3df(50.0f, 30.0f, 20.0f) << 50.0f;

			// Compress the packet, not much to be said.
			packet.compressPacket();

			// Encrypt the packet. Note that here we are just using a simple key
			// that is shared among the client and the server. In more sophisticated
			// implementations you may want to generate a random key on the server for
			// each client and send that using a shared key, then use the new key for
			// further communication. Remember that the key should be 16 characters
			// long, and obviously the client and server must share the same key.
			packet.encryptPacket("hushthisissecret");

			// A simple call to "sendOutPacket" will send the packet to the server.
			netManager->sendOutPacket(packet);

			netManager->update(1000);
			i++;
		}

		// When the connection disconnects (Or fails), the loop will terminate.
		// Remember to delete the netManager so that all the low level networking
		// stuff is cleaned up properly.
		delete netManager;
	}

	// And we're done, return 0 and make like a tree.
	return 0;
}
