#pragma once
#include <vector>
#include <queue>
#include <cstdint>

#include "RemotePeer.h"

namespace NetLib
{
	class Address;

	enum ConnectionFailedReasonType : uint8_t
	{
		CFR_UNKNOWN = 0,			//Unexpect
		CFR_TIMEOUT = 1,			//The peer is inactive
		CFR_SERVER_FULL = 2,		//The server can't handle more connections, it has reached its maximum
		CFR_PEER_SHUT_DOWN = 3,		//The peer has shut down its Network system
		CFR_CONNECTION_TIMEOUT = 4	//The in process connection has taken too long
	};

	struct RemotePeerDisconnectionData
	{
		unsigned int index;
		bool shouldNotify;
		ConnectionFailedReasonType reason;
	};

	//If the local peer does not receive any message from a remote peer during REMOTE_PEER_INACTIVITY_TIME seconds it will be considered inactive and it will
	//be disconnected with ConnectionFailedReasonType::CFR_TIMEOUT reason
	const float REMOTE_PEER_INACTIVITY_TIME = 5.0f;

	class RemotePeersHandler
	{
	public:
		RemotePeersHandler(unsigned int maxConnections);

		void TickRemotePeers(float elapsedTime);

		bool AddRemotePeer(const Address& addressInfo, uint16_t id, uint64_t dataPrefix);
		int FindFreeRemotePeerSlot() const;
		RemotePeer* GetRemotePeerFromAddress(const Address& address);
		int GetRemotePeerIndexFromAddress(const Address& address) const;
		int GetRemotePeerIndex(const RemotePeer& remotePeer) const;
		bool IsRemotePeerAlreadyConnected(const Address& address) const;

		void RemoveAllRemotePeers();
		void RemoveRemotePeer(unsigned int remotePeerIndex);

		~RemotePeersHandler();

	private:
		const int _maxConnections;
		std::vector<bool> _remotePeerSlots;
		std::vector<RemotePeer> _remotePeers;
	};
}