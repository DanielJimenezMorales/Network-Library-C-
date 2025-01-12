#include "Scene.h"

#include <cassert>

#include "GameEntity.hpp"
#include "IPreTickSystem.h"

#include "components/transform_component.h"

#include "entity_factories/i_entity_factory.h"

Scene::Scene()
    : _entityContainer()
    , _systemsHandler()
{
}

bool Scene::RegisterEntityFactory( const std::string& id, IEntityFactory* factory )
{
	assert( factory != nullptr );

	auto id_found = _entityFactories.find( id );
	if ( id_found != _entityFactories.cend() )
	{
		return false;
	}

	_entityFactories.insert( { id, factory } );
	return true;
}

void Scene::AddSystem( ECS::SystemCoordinator* system )
{
	_systemsHandler.AddSystem( system );
}

void Scene::Update( float32 elapsed_time )
{
	_systemsHandler.TickStage( _entityContainer, elapsed_time, ECS::ExecutionStage::UPDATE );
}

void Scene::PreTick( float32 elapsed_time )
{
	_systemsHandler.TickStage( _entityContainer, elapsed_time, ECS::ExecutionStage::PRETICK );

	auto it = _preTickSystems.begin();
	for ( ; it != _preTickSystems.end(); ++it )
	{
		( *it )->PreTick( _entityContainer, elapsed_time );
	}
}

void Scene::Tick( float32 elapsed_time )
{
	_systemsHandler.TickStage( _entityContainer, elapsed_time, ECS::ExecutionStage::TICK );
}

void Scene::PosTick( float32 elapsed_time )
{
	_systemsHandler.TickStage( _entityContainer, elapsed_time, ECS::ExecutionStage::POSTICK );
}

void Scene::Render( float32 elapsed_time )
{
	_systemsHandler.TickStage( _entityContainer, elapsed_time, ECS::ExecutionStage::RENDER );
}

void Scene::EndOfFrame()
{
	DestroyPendingEntities();
}

void Scene::AddPreTickSystem( IPreTickSystem* system )
{
	assert( system != nullptr );
	_preTickSystems.push_back( system );
}

GameEntity Scene::CreateGameEntity()
{
	GameEntity newEntity = _entityContainer.CreateGameEntity();
	newEntity.AddComponent< TransformComponent >();

	return newEntity;
}

GameEntity Scene::CreateGameEntity( const std::string& type, const BaseEntityConfiguration* config )
{
	auto factory_found = _entityFactories.find( type );
	if ( factory_found == _entityFactories.cend() )
	{
		return GameEntity();
	}

	GameEntity new_entity = _entityContainer.CreateGameEntity();
	IEntityFactory* factory = factory_found->second;
	factory->Create( new_entity, config );
	_onEntityCreate.Execute( new_entity );
	return new_entity;
}

void Scene::DestroyGameEntity( const GameEntity& entity )
{
	_entitiesToRemoveRequests.push( entity.GetId() );
}

GameEntity Scene::GetEntityFromId( uint32 id )
{
	return _entityContainer.GetEntityFromId( id );
}

void Scene::UnsubscribeFromOnEntityCreate( uint32 id )
{
	_onEntityCreate.DeleteSubscriber( id );
}

void Scene::UnsubscribeFromOnEntityDestroy( uint32 id )
{
	_onEntityDestroy.DeleteSubscriber( id );
}

void Scene::DestroyPendingEntities()
{
	while ( !_entitiesToRemoveRequests.empty() )
	{
		const ECS::EntityId entity_id = _entitiesToRemoveRequests.front();
		_entitiesToRemoveRequests.pop();

		GameEntity entity = _entityContainer.GetEntityFromId( entity_id );
		_onEntityDestroy.Execute( entity );
		_entityContainer.DestroyGameEntity( entity_id );
	}
}
