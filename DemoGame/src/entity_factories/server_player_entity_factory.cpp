#include "server_player_entity_factory.h"

#include "entity_configurations/server_player_entity_configuration.h"

#include "components/transform_component.h"
#include "components/sprite_renderer_component.h"
#include "components/collider_2d_component.h"
#include "components/network_entity_component.h"
#include "components/player_controller_component.h"

#include "player_network_entity_serialization_callbacks.h"

#include "replication/network_entity_communication_callbacks.h"

#include "CircleBounds2D.h"

#include "GameEntity.hpp"
#include "ServiceLocator.h"
#include "TextureLoader.h"

ServerPlayerEntityFactory::ServerPlayerEntityFactory()
    : IEntityFactory()
{
}

void ServerPlayerEntityFactory::Create( GameEntity& entity, const BaseEntityConfiguration* configuration )
{
	const ServerPlayerEntityConfiguration& casted_config =
	    static_cast< const ServerPlayerEntityConfiguration& >( *configuration );

	ServiceLocator& serviceLocator = ServiceLocator::GetInstance();
	ITextureLoader& textureLoader = serviceLocator.GetTextureLoader();
	Texture* texture = textureLoader.LoadTexture( "sprites/PlayerSprites/playerHead.png" );

	entity.AddComponent< TransformComponent >( casted_config.position, casted_config.lookAt );
	entity.AddComponent< SpriteRendererComponent >( texture );

	CircleBounds2D* circleBounds2D = new CircleBounds2D( 5.f );
	entity.AddComponent< Collider2DComponent >( circleBounds2D, false, CollisionResponseType::Dynamic );

	entity.AddComponent< NetworkEntityComponent >( casted_config.networkEntityId, casted_config.controlledByPeerId );

	PlayerControllerConfiguration playerConfiguration;
	playerConfiguration.movementSpeed = 25;

	entity.AddComponent< PlayerControllerComponent >( nullptr, casted_config.networkEntityId, playerConfiguration );

	// Subscribe to Serialize for owner
	auto serialize_owner_callback = [ entity ]( NetLib::Buffer& buffer )
	{
		SerializeForOwner( entity, buffer );
	};
	casted_config.communicationCallbacks->OnSerializeEntityStateForOwner.AddSubscriber( serialize_owner_callback );

	// Subscribe to Serialize for non owner
	auto serialize_non_owner_callback = [ entity ]( NetLib::Buffer& buffer )
	{
		SerializeForNonOwner( entity, buffer );
	};
	casted_config.communicationCallbacks->OnSerializeEntityStateForNonOwner.AddSubscriber(
	    serialize_non_owner_callback );
}

void ServerPlayerEntityFactory::Destroy( GameEntity& entity )
{
}
