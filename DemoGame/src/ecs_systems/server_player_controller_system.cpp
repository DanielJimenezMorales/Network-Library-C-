#include "server_player_controller_system.h"

#include "GameEntity.hpp"
#include "InputState.h"
#include "PlayerSimulator.h"

#include "ecs/entity_container.h"

#include "components/player_controller_component.h"
#include "components/network_peer_component.h"
#include "components/network_entity_component.h"

ServerPlayerControllerSystem::ServerPlayerControllerSystem()
    : ECS::ISimpleSystem()
{
}

void ServerPlayerControllerSystem::Execute( std::vector< GameEntity >& entities, ECS::EntityContainer& entity_container,
                                            float32 elapsed_time )
{
	GameEntity& networkPeerEntity = entity_container.GetFirstEntityOfType< NetworkPeerComponent >();
	NetworkPeerComponent& networkPeerComponent = networkPeerEntity.GetComponent< NetworkPeerComponent >();
	NetLib::Server* serverPeer = networkPeerComponent.GetPeerAsServer();

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