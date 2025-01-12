#include "server_player_controller_system.h"

#include "GameEntity.hpp"
#include "InputState.h"
#include "PlayerSimulator.h"

#include "ecs/entity_container.h"

#include "components/player_controller_component.h"
#include "components/network_entity_component.h"

#include "global_components/network_peer_global_component.h"

ServerPlayerControllerSystem::ServerPlayerControllerSystem()
    : ECS::ISimpleSystem()
{
}

void ServerPlayerControllerSystem::Execute( ECS::EntityContainer& entity_container, float32 elapsed_time )
{
	NetworkPeerGlobalComponent& networkPeerComponent =
	    entity_container.GetGlobalComponent< NetworkPeerGlobalComponent >();
	NetLib::Server* serverPeer = networkPeerComponent.GetPeerAsServer();

	std::vector< GameEntity > entities = entity_container.GetEntitiesOfType< PlayerControllerComponent >();
	for ( auto it = entities.begin(); it != entities.end(); ++it )
	{
		const NetworkEntityComponent& networkEntityComponent = it->GetComponent< NetworkEntityComponent >();
		const NetLib::IInputState* baseInputState =
		    serverPeer->GetInputFromRemotePeer( networkEntityComponent.controlledByPeerId );
		if ( baseInputState == nullptr )
		{
			return;
		}

		const InputState* inputState = static_cast< const InputState* >( baseInputState );
		PlayerSimulator::Simulate( *inputState, *it, elapsed_time );
	}
}
