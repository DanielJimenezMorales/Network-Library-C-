#include "NetworkSystem.h"

#include "logger.h"

#include "core/client.h"
#include "core/server.h"

#include "GameEntity.hpp"

#include "ecs/entity_container.h"

#include "components/network_peer_component.h"

#include <vector>

void NetworkSystem::PreTick( ECS::EntityContainer& entityContainer, float32 elapsedTime ) const
{
	GameEntity networkPeerEntity = entityContainer.GetFirstEntityOfType< NetworkPeerComponent >();
	NetworkPeerComponent& networkPeerComponent = networkPeerEntity.GetComponent< NetworkPeerComponent >();

	if ( networkPeerComponent.peer->GetConnectionState() == NetLib::PCS_Disconnected )
	{
		networkPeerComponent.peer->Start();
	}
	else
	{
		networkPeerComponent.peer->PreTick();
	}

	// Process new remote peer connections
	while ( !networkPeerComponent.unprocessedConnectedRemotePeers.empty() )
	{
		uint32 unprocessedConnectedRemotePeerId = networkPeerComponent.unprocessedConnectedRemotePeers.front();
		networkPeerComponent.unprocessedConnectedRemotePeers.pop();
		Server_SpawnRemotePeerConnect( entityContainer, unprocessedConnectedRemotePeerId );
	}
}

void NetworkSystem::PosTick( ECS::EntityContainer& entityContainer, float32 elapsedTime ) const
{
	GameEntity networkPeerEntity = entityContainer.GetFirstEntityOfType< NetworkPeerComponent >();
	NetworkPeerComponent& networkPeerComponent = networkPeerEntity.GetComponent< NetworkPeerComponent >();
	networkPeerComponent.peer->Tick( elapsedTime );
}

void NetworkSystem::Server_SpawnRemotePeerConnect( ECS::EntityContainer& entityContainer, uint32 remotePeerId ) const
{
	// Spawn its local player entity:
	GameEntity networkPeerEntity = entityContainer.GetFirstEntityOfType< NetworkPeerComponent >();
	NetworkPeerComponent& networkPeerComponent = networkPeerEntity.GetComponent< NetworkPeerComponent >();
	NetLib::Server* serverPeer = networkPeerComponent.GetPeerAsServer();
	serverPeer->CreateNetworkEntity( 10, remotePeerId, 0.f, 0.f );
}
