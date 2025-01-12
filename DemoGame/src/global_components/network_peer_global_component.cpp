#include "network_peer_global_component.h"

#include "inputs/i_input_state_factory.h"

#include "replication/i_network_entity_factory.h"

NetworkPeerGlobalComponent::~NetworkPeerGlobalComponent()
{
	if ( isTrackingRemotePeerConnect )
	{
		peer->UnsubscribeToOnPeerConnected( remotePeerConnectSubscriptionId );
	}

	delete peer;
	peer = nullptr;
	auto it = networkEntityFactories.begin();
	for ( ; it != networkEntityFactories.end(); ++it )
	{
		delete *it;
		*it = nullptr;
	}

	if ( inputStateFactory != nullptr )
	{
		delete inputStateFactory;
		inputStateFactory = nullptr;
	}

	networkEntityFactories.clear();
}
