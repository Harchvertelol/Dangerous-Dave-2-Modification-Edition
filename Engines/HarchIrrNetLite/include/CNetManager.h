#ifndef IRRNET_CNM_H
#define IRRNET_CNM_H

#include <wchar.h>
#include <iostream>
#include <string>
#include <vector>

#include "INetManager.h"
#include "SPacket.h"
#include "enet/enet.h"

using namespace irr;
using namespace core;

namespace irr
{
	namespace net
	{

		class CNetManager : public INetManager
		{
		public:
			/// This constructor sets up a server, as it only takes a port as a parameter to bind the host to.
			CNetManager(INetCallback* handler, u32 const port = 45000, const SNetParams& params = SNetParams());

			/// This constructor sets up a client, as it takes an address and a port to connect to. Connecting
			/// will commence immediately after the INetManager is initialised.
			CNetManager(INetCallback* handler, const c8* addressc, const u32 port = 45000, const SNetParams& params = SNetParams());

			/// Destructor
			~CNetManager();

			virtual void update(const u32 timeOut = 0);
			virtual void setVerbose(bool isverbose);
			virtual void setNetIterations(u16 iterations) {netIterations = iterations;}
			virtual void setNetCallback(INetCallback* netCallback) {pHandler = netCallback;}
			virtual void setGlobalPacketRelay(bool relay);
            virtual void sendOutPacket(SOutPacket& outpacket, const s32 playerId = -1, const u32 channelID = 0);
			virtual void sendOutPacketUnreliable(SOutPacket& outpacket, const s32 playerId = -1, const u32 channelID = 0, bool isUnsequenced = false);
			virtual void kickClient(const u16 playerId, bool hardKick = false);

			virtual const u32 getPeerCount();
			virtual const u32 getChannelsCount();
			virtual const u32 getPing(u32 pID = 0);
			virtual const u32 getClientAddress(const u16 playerId);

			virtual const u16 getPlayerNumber();

			virtual E_IRRNET_CONNECTION_STATUS getConnectionStatus();

			enum E_NET_MODE
			{
				ENM_SERVER,
				ENM_CLIENT,

				ENM_COUNT
			};

			struct SPeerData
			{
				SPeerData(u16 pID, ENetPeer* enetPeerIn) : playerID(pID), enetPeer(enetPeerIn) {};

				u16 playerID;
				ENetPeer* enetPeer;
			};

	private:
			ENetHost* host;
			ENetPeer* peer;
			ENetAddress address;

			u16 playerID;

			INetCallback* pHandler;
			u32 netIterations;
			SNetParams netParams;

			std::vector<SPeerData*> players;
			bool verbose;
			bool globPacketRelay;

			E_IRRNET_CONNECTION_STATUS connectionStatus;
			E_NET_MODE mode;

			bool setUpClient(const c8* addressc, u32 port);
			bool setUpServer(u32 port);

			ENetPeer* getPlayerById(u16 id);
			ENetPeer* getPeer(u32 pID);
			ENetHost* getHost();
		};

	} // Close Net Namespace
} // Close Irr namespace

#endif

// Copyright(C) Ahmed Hilali 2007

/* License:

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
