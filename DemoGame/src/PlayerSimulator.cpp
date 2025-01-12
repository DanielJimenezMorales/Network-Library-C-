#include "PlayerSimulator.h"
#include "InputState.h"
#include "GameEntity.hpp"
#include "PlayerControllerConfiguration.h"
#include "Logger.h"

#include "components/transform_component.h"
#include "components/player_controller_component.h"

void PlayerSimulator::Simulate( const InputState& inputs, GameEntity& playerEntity, float32 elapsedTime )
{
	TransformComponent& transform = playerEntity.GetComponent< TransformComponent >();
	if ( inputs.movement.X() != 0 || inputs.movement.Y() != 0 )
	{
		LOG_WARNING( "MOVEMENT {x: %f, y: %f}", inputs.movement.X(), inputs.movement.Y() );
	}
	PlayerControllerComponent& networkComponent = playerEntity.GetComponent< PlayerControllerComponent >();
	Vec2f updatedPosition = UpdatePosition( inputs.movement, transform, networkComponent.configuration, elapsedTime );
	ApplyPosition( updatedPosition, transform );

	transform.SetPosition( updatedPosition );
	networkComponent.posX = updatedPosition.X();
	networkComponent.posY = updatedPosition.Y();

	Vec2f updatedLookAt =
	    UpdateLookAt( inputs.virtualMousePosition, transform, networkComponent.configuration, elapsedTime );
	transform.LookAt( inputs.virtualMousePosition );

	networkComponent.rotationAngle = transform.GetRotationAngle();
}

Vec2f PlayerSimulator::UpdatePosition( const Vec2f& inputs, const TransformComponent& transform,
                                       const PlayerControllerConfiguration& configuration, float32 elapsedTime )
{
	Vec2f currentPosition = transform.GetPosition();

	currentPosition.AddToX( inputs.X() * configuration.movementSpeed * elapsedTime );
	currentPosition.AddToY( inputs.Y() * configuration.movementSpeed * elapsedTime );

	return currentPosition;
}

void PlayerSimulator::ApplyPosition( const Vec2f& position, TransformComponent& transform )
{
	transform.SetPosition( position );
}

Vec2f PlayerSimulator::UpdateLookAt( const Vec2f& virtualMousePosition, const TransformComponent& transform,
                                     const PlayerControllerConfiguration& configuration, float32 elapsedTime )
{
	return Vec2f();
}
