#pragma once
#include <cassert>
#include <vector>

#include "core/peer.h"
#include "core/client.h"
#include "core/server.h"

#include <queue>

class INetworkEntityFactory;
class IInputStateFactory;

struct NetworkPeerGlobalComponent
{
	NetworkPeerGlobalComponent() : peer(nullptr), networkEntityFactories(), inputStateFactory(nullptr), unprocessedConnectedRemotePeers(), isTrackingRemotePeerConnect(false), remotePeerConnectSubscriptionId(0)
	{
	}

	~NetworkPeerGlobalComponent();

	//TODO free this memory correctly
	NetLib::Peer* peer;
	std::vector<INetworkEntityFactory*> networkEntityFactories;
	NetLib::IInputStateFactory* inputStateFactory;

	std::queue<uint32> unprocessedConnectedRemotePeers;
	bool isTrackingRemotePeerConnect;
	uint32 remotePeerConnectSubscriptionId;

	void TrackOnRemotePeerConnect()
	{
		auto callback = [&](uint32 remotePeerId) {this->AddUnprocessedConnectedRemotePeer(remotePeerId); };
		remotePeerConnectSubscriptionId = peer->SubscribeToOnRemotePeerConnect(callback);
		isTrackingRemotePeerConnect = true;
	}

	void AddUnprocessedConnectedRemotePeer(uint32 remotePeerId) { unprocessedConnectedRemotePeers.push(remotePeerId); };

	NetLib::Client* GetPeerAsClient() const { assert(peer != nullptr); assert(peer->GetPeerType() == NetLib::PeerType::CLIENT); return static_cast<NetLib::Client*>(peer); }
	NetLib::Server* GetPeerAsServer() const { assert(peer != nullptr); assert(peer->GetPeerType() == NetLib::PeerType::SERVER); return static_cast<NetLib::Server*>(peer); }
};
