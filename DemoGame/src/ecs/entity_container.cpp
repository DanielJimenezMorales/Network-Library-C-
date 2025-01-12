#include "entity_container.h"

#include "GameEntity.hpp"

namespace ECS
{
	EntityContainer::EntityContainer()
	    : _entities()
	    , _globalEntityId( 0 )
	{
		const entt::entity id = _entities.create();
		_globalEntityId = static_cast< EntityId >( id );
	}

	GameEntity EntityContainer::CreateGameEntity()
	{
		const entt::entity entityId = _entities.create();
		return GameEntity( static_cast< EntityId >( entityId ), this );
	}

	void EntityContainer::DestroyGameEntity( EntityId id )
	{
		_entities.destroy( static_cast< entt::entity >( id ) );
	}

	GameEntity EntityContainer::GetEntityFromId( uint32 id )
	{
		GameEntity result;

		entt::entity entt_id = static_cast< entt::entity >( id );
		if ( _entities.valid( entt_id ) )
		{
			result._ecsEntityId = static_cast< EntityId >( entt_id );
			result._entityContainer.Set( this );
		}
		return result;
	}
} // namespace ECS
