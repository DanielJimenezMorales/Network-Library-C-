#pragma once
#include "numeric_types.h"
#include "delegate.h"

#include <vector>
#include <queue>
#include <unordered_map>

#include "ecs/entity_container.h"
#include "ecs/systems_handler.h"

class GameEntity;
class IPreTickSystem;

struct BaseEntityConfiguration;
class IEntityFactory;

class Scene
{
	public:
		Scene();

		bool RegisterEntityFactory( const std::string& id, IEntityFactory* factory );

		void AddSystem( ECS::SystemCoordinator* system );

		template < typename T, typename... Params >
		T& AddGlobalComponent( Params&&... params );

		void Update( float32 elapsed_time );
		void PreTick( float32 elapsed_time );
		void Tick( float32 elapsed_time );
		void PosTick( float32 elapsed_time );
		void Render( float32 elapsed_time );
		void EndOfFrame();

		void AddPreTickSystem( IPreTickSystem* system );

		GameEntity CreateGameEntity();
		GameEntity CreateGameEntity( const std::string& type, const BaseEntityConfiguration* config );
		void DestroyGameEntity( const GameEntity& entity );

		template < typename T >
		GameEntity GetFirstEntityOfType();
		GameEntity GetEntityFromId( uint32 id );

		template < typename Functor >
		uint32 SubscribeToOnEntityCreate( Functor&& functor );
		void UnsubscribeFromOnEntityCreate( uint32 id );

		template < typename Functor >
		uint32 SubscribeToOnEntityDestroy( Functor&& functor );
		void UnsubscribeFromOnEntityDestroy( uint32 id );

	private:
		void DestroyPendingEntities();

		ECS::EntityContainer _entityContainer;
		ECS::SystemsHandler _systemsHandler;

		std::vector< IPreTickSystem* > _preTickSystems;

		std::queue< ECS::EntityId > _entitiesToRemoveRequests;

		std::unordered_map< std::string, IEntityFactory* > _entityFactories;

		Common::Delegate< GameEntity& > _onEntityCreate;
		Common::Delegate< GameEntity& > _onEntityDestroy;
};

template < typename T, typename... Params >
inline T& Scene::AddGlobalComponent( Params&&... params )
{
	return _entityContainer.AddGlobalComponent< T >( std::forward< Params >( params )... );
}

template < typename T >
inline GameEntity Scene::GetFirstEntityOfType()
{
	return _entityContainer.GetFirstEntityOfType< T >();
}

template < typename Functor >
inline uint32 Scene::SubscribeToOnEntityCreate( Functor&& functor )
{
	return _onEntityCreate.AddSubscriber( std::forward< Functor >( functor ) );
}

template < typename Functor >
inline uint32 Scene::SubscribeToOnEntityDestroy( Functor&& functor )
{
	return _onEntityDestroy.AddSubscriber( std::forward< Functor >( functor ) );
}
