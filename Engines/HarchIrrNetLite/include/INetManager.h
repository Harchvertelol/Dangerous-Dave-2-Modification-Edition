#ifndef IRRNET_INM_H
#define IRRNET_INM_H

#include "SPacket.h"

/** \mainpage

Welcome to irrNetLite.

*/

namespace irr
{
namespace net
{

/**
E_IRRNET_CONNECTION_STATUS:

EICS_PENDING
------------
The connection is still in progress.

EICS_ESTABLISHED
------------
The connection has been successfully established.

EICS_FAILED
------------
The connection has failed. The current INetManager is invalid. To retry, you must create a new NetManager.
*/
enum E_IRRNET_CONNECTION_STATUS
{
	EICS_PENDING,
	EICS_ESTABLISHED,
	EICS_FAILED,

	EICS_COUNT
};

/**
Optional settings for the creation of the INetManager.

downBandwidth
-------------
The down (incoming) bandwidth of the current internet connection. You may set this value for
more accurate packet throttling. In kbps.

upBandwidth
-----------
The up (outgoing) bandwidth of the current internet connection. You may set this value for
more accurate packet throttling. In kbps.

maxClients
----------
The maximum number of clients allowed for a server. Any clients that attempt to join
after this number has been reached will be rejected. (For servers only.)

connectionTimeout
-----------------
The maximum connection timeout allowed when connecting to a server. In milliseconds. (For
clients only.)

numberChannels
-----------------
The number of ENet channels for connection.
Attention: channelID with number 1 used for service information. If it value is 1, we have
only one channel with ID 0. If channelID == 2, we have two channels: first with channelID == 0
and second with channelID == 2 (not 1). If numberChannels == 3, we have three channels with
IDs 0, 2 and 3. Channel with number 1 is service channel! Maybe in next versions I change service
channel from 1 to 0, but not now.
*/
struct SNetParams
{
	SNetParams() : downBandwidth(128000), upBandwidth(56000), maxClients(100), connectionTimeout(5000), numberChannels(1) {};

	u32 downBandwidth;
	u32 upBandwidth;
	u32 maxClients;
	u32 connectionTimeout;
	u32 numberChannels;
};

/**
You must create a derived class of INetCallback and override the "handlePacket" method to perform
your own packet handling functions. Everytime a packet is recieved over the network, an SInPacket
is passed to this method with channelID (it is ENet channel ID). When initialising a INetManager, a pointer to an instance of the
derived class must be passed. Please see SInPacket regarding information on handling InPackets.
SInPacket with channelID == 1 not passed to handlePacket, it is service channel for IrrNetLite.
*/
class INetCallback
{
public:
	virtual void handlePacket(SInPacket& packet, u32 channelID) = 0;
	virtual void onConnect(const u16 playerId) {};
	virtual void onDisconnect(const u16 playerId) {};

	virtual ~INetCallback() {};
};

/**
INetManager class.
*/
class INetManager
{
public:
	/**
	This function updates the state of the NetManager and handles the sending/receiving of packets.
	Make sure to stick this in your run loop, or at the very least update it very often. If this
	function is not updated pending packets will not be sent and received packets will not be handled.
	Optionally a timeout may be specified if you want this function to block and wait for packets
	to a arrive for a specified interval of time (In MilliSeconds).
	*/
	virtual void update(const u32 timeOut = 0) = 0;

	/// Virtual destructor
	virtual ~INetManager() {};

	/**
	If set to true INetManager will output debug info to console. Output resembles:

	"irrNetLite: A packet of length 50 was recieved.";

	   or

	"irrNetLite: Player number 23 disconnected.";

	It may be useful for debugging purposes.
	*/
	virtual void setVerbose(bool isverbose) = 0;

	/// Sets a new INetCallback as the net callback.
	virtual void setNetCallback(INetCallback* netCallback) = 0;

	/**
	This function sends an SOutPacket. Out packets provide an easy interface for sending data across a network.
	Conversions etc are all taken care of by irrNet. All you have to do is create some kind of variable then add it,
	then read the data from the SInPacket on the recieving end. Here is an example of adding data to an SOutPacket:

	\code
	SOutPacket MyOutPacket;
	// We add data using the "<<" operator, similar to iostreams.
	float myValue = 0.238723f;
	MyOutPacket << myValue;
	MyOutPacket << vector3df(23,354,35);
	MyOutPacket << 43988434;

	// And now to send.
	NetManager->sendOutPacket(MyOutPacket);
	\endcode

	This function send packet to ENet channel with channelID (default 0).

	If playerId < 0 (default == -1) this sends the packet to all connected clients if you are the server, or directly to the server if you
	are a client. To send to a specific client set playerId to ID of player.
	Please refer to SOutPacket for more information on what types of variables an SOutPacket can handle
	automatically.

	If playerId >= 0, this sends an SOutPacket to a specific player denoted by the player ID (number). Players are numbered
	automatically as they connect to the server. IMPORTANT: This feature is only valid if you are the server.
	This is because clients can only send packets to the server, making this function unusable.
	Please read above for more info.

	Example sending from client to channel 2:

	NetManager->sendOutPacket(MyOutPacket, -1, 2);

	*/
	virtual void sendOutPacket(SOutPacket& outpacket, const s32 playerId = -1, const u32 channelID = 0) = 0;

	/**
	If playerId < 0 (default == -1) this sends the SOutPacket specified unreliably. This is not recommended for critical information.

	If playerId >= 0 this sends an SOutPacket unreliably to a specific player denoted by the player ID (number).
	Players are numbered automatically as they connect to the server. IMPORTANT: This feature
	is only valid if you are the server. This is because clients can only send packets to the server,
	making this function unusable. This is not recommended for critical information.
	If isUnsequenced == 0, this packet will not be sequenced with other packets.
	Please refer to sendOutPacket(SOutPacket& outpacket, const s32 playerId = -1, const u32 channelID = 0) for more info.
	*/
	virtual void sendOutPacketUnreliable(SOutPacket& outpacket, const s32 playerId = -1, const u32 channelID = 0, bool isUnsequenced = false) = 0;

	/// This gets the number of players connected. This is only valid for servers.
	virtual const u32 getPeerCount() = 0;

	/// This gets the number of ENet channels.
	virtual const u32 getChannelsCount() = 0;

	/// This returns the playerID for this machine. Only valid for Clients.
	virtual const u16 getPlayerNumber() = 0;

	/// Returns the client ip address with the specified player ID. This is only valid for servers.
	/// The address is in 32-bit integer format.
	virtual const u32 getClientAddress(const u16 playerId) = 0;

	/// Kicks the client with the specified player Id.
	/// You may set the hardKick option to true to forcifully disconnect them immediately.
	/// Note that if a "hardKick" is performed, no disconnect event will be generated.
	virtual void kickClient(const u16 playerId, bool hardKick = false) = 0;

	/**
	This returns a value of type E_IRRNET_CONNECTION_STATUS. This can either be EICS_PENDING, which indicates the
	connection is still in process, EICS_ESTABLISHED, which indicates the connection has been successfully established,
	or EICS_FAILED, which indicates the connection was not successful or has since disconnected. This is generally only
	useful for clients, but the information is provided for servers incase there is a fault with the server setup.
	*/
	virtual E_IRRNET_CONNECTION_STATUS getConnectionStatus() = 0;

	/// This returns the ping of player with id pID (for server). For clients as it always returns the ping to the server.
	virtual const u32 getPing(u32 pID = 0) = 0;

	/// Sets the number of packet processing iterations per update. You may want to increase this number if
	/// you are not updating frequently enough for all the packets to get processed. (Default: 10000)
	virtual void setNetIterations(u16 iterations) = 0;

	/**
	Enables or disables global packet relay, this is when the server relays all the packets it recieves to all
	connected clients, this makes it easy to send information to all clients, but may not be suitable for
	large-scale applications. Instead, a custom entity management and packet handling system should be designed.
	Although for applications with less than 10 peers global packet relay should do just fine. (Default: false)
	*/
	virtual void setGlobalPacketRelay(bool relay) = 0;
};

/// Creates an irrNetLite client.
INetManager* createIrrNetClient(INetCallback* callback, const c8* addressc, const u32 port = 45000, const SNetParams& params = SNetParams());

/// Creates an irrNetLite server.
INetManager* createIrrNetServer(INetCallback* callback, const u32 port = 45000, const SNetParams& params = SNetParams());

} // Close Net Namespace
} // Close Irr namespace

#endif

// Copyright(C) Ahmed Hilali 2008

/*
	License:
		This software is provided 'as-is', without any express or implied
		warranty.  In no event will the authors be held liable for any damages
		arising from the use of this software.

		Permission is granted to anyone to use this software for any purpose,
		including commercial applications, and to alter it and redistribute it
		freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented; you must not
		 claim that you wrote the original software. If you use this software
		 in a product, an acknowledgement in the product documentation would be
		 appreciated but is not required.
		2. Altered source versions must be clearly marked as such, and must not be
		 misrepresented as being the original software.
		3. This notice may not be removed or altered from any source distribution.
*/

