#pragma once
#include <cassert>

#include "safe_pointer.hpp"

#include "entt.hpp"

#include "ecs/entity_container.h"

// TODO improve GameEntity
class GameEntity
{
	public:
		GameEntity()
		    : _ecsEntityId()
		    , _entityContainer( nullptr ){};
		GameEntity( ECS::EntityId enttId, ECS::EntityContainer* entityContainer )
		    : _ecsEntityId( enttId )
		    , _entityContainer( entityContainer ){};
		GameEntity( const GameEntity& other ) = default;
		~GameEntity(){};

		GameEntity& operator=( const GameEntity& other )
		{
			if ( this == &other )
			{
				return *this;
			}

			_ecsEntityId = other._ecsEntityId;
			_entityContainer = other._entityContainer;
			return *this;
		}

		bool operator==( const GameEntity& other )
		{
			return _ecsEntityId == other._ecsEntityId && _entityContainer == other._entityContainer;
		}
		bool operator!=( const GameEntity& other ) { return !( *this == other ); }

		ECS::EntityId GetId() const { return _ecsEntityId; }

		ECS::EntityContainer* GetEntityContainer() { return _entityContainer.GetRawPointer(); }
		const ECS::EntityContainer* GetEntityContainer() const { return _entityContainer.GetRawPointer(); }

		bool IsValid() const { return _entityContainer != nullptr; };

		template < typename T, typename... Params >
		T& AddComponent( Params&&... params );

		template < typename T >
		bool HasComponent();

		template < typename T >
		T& GetComponent();

		template < typename T >
		const T& GetComponent() const;

		template < typename T >
		void RemoveComponent();

	private:
		ECS::EntityId _ecsEntityId;

		// TODO Add SafePointer wrapper
		SafePointer< ECS::EntityContainer > _entityContainer;

		friend class ECS::EntityContainer;
};

template < typename T, typename... Params >
inline T& GameEntity::AddComponent( Params&&... params )
{
	assert( IsValid() );
	return _entityContainer->AddComponentToEntity< T >( *this, std::forward< Params >( params )... );
};

template < typename T >
inline bool GameEntity::HasComponent()
{
	assert( IsValid() );
	return _entityContainer->HasEntityComponent< T >( *this );
};

template < typename T >
inline T& GameEntity::GetComponent()
{
	assert( IsValid() );
	return _entityContainer->GetComponentFromEntity< T >( *this );
}
template < typename T >
inline const T& GameEntity::GetComponent() const
{
	assert( IsValid() );
	return _entityContainer->GetComponentFromEntity< T >( *this );
}

template < typename T >
inline void GameEntity::RemoveComponent()
{
	assert( IsValid() );
	_entityContainer->RemoveComponentFromEntity( *this );
};
