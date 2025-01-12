#include "NetworkEntityFactory.h"

#include "GameEntity.hpp"
#include "Scene.h"
#include "core/client.h"
#include "player_network_entity_serialization_callbacks.h"

#include "Vec2f.h"

#include "replication/network_entity_communication_callbacks.h"

#include "entity_configurations/server_player_entity_configuration.h"
#include "entity_configurations/client_local_player_entity_configuration.h"
#include "entity_configurations/client_remote_entity_configuration.h"

#include "global_components/network_peer_global_component.h"

void NetworkEntityFactory::SetScene( Scene* scene )
{
	_scene = scene;
}

// TODO Not useful anymore. Delete
void NetworkEntityFactory::SetPeerType( NetLib::PeerType peerType )
{
	_peerType = peerType;
}

int32 NetworkEntityFactory::CreateNetworkEntityObject(
    uint32 networkEntityType, uint32 networkEntityId, uint32 controlledByPeerId, float32 posX, float32 posY,
    NetLib::NetworkEntityCommunicationCallbacks& communication_callbacks )
{
	int32 id;

	if ( _peerType == NetLib::PeerType::SERVER )
	{
		ServerPlayerEntityConfiguration config;
		config.position = Vec2f( posX, posY );
		config.lookAt = 0.f;
		config.networkEntityId = networkEntityId;
		config.controlledByPeerId = controlledByPeerId;
		config.communicationCallbacks = &communication_callbacks;

		const GameEntity entity = _scene->CreateGameEntity( "PLAYER", &config );
		id = entity.GetId();
	}
	else if ( _peerType == NetLib::PeerType::CLIENT )
	{
		const GameEntity network_peer_entity = _scene->GetFirstEntityOfType< NetworkPeerGlobalComponent >();
		const NetworkPeerGlobalComponent& network_peer_component = network_peer_entity.GetComponent< NetworkPeerGlobalComponent >();
		const NetLib::Client* clientPeer = static_cast< NetLib::Client* >( network_peer_component.peer );
		if ( clientPeer->GetLocalClientId() == controlledByPeerId )
		{
			ClientLocalPlayerEntityConfiguration config;
			config.position = Vec2f( posX, posY );
			config.lookAt = 0.f;
			config.networkEntityId = networkEntityId;
			config.controlledByPeerId = controlledByPeerId;
			config.communicationCallbacks = &communication_callbacks;

			const GameEntity entity = _scene->CreateGameEntity( "LOCAL_PLAYER", &config );
			id = entity.GetId();
		}
		else
		{
			ClientRemoteEntityConfiguration config;
			config.position = Vec2f( posX, posY );
			config.lookAt = 0.f;
			config.networkEntityId = networkEntityId;
			config.controlledByPeerId = controlledByPeerId;
			config.communicationCallbacks = &communication_callbacks;

			const GameEntity entity = _scene->CreateGameEntity( "REMOTE_PLAYER", &config );
			id = entity.GetId();
		}
	}

	return id;
}

void NetworkEntityFactory::DestroyNetworkEntityObject( uint32 gameEntity )
{
	const GameEntity entity = _scene->GetEntityFromId( gameEntity );
	_scene->DestroyGameEntity( entity );
}
