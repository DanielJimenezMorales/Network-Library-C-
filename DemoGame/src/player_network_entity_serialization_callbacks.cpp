#include "player_network_entity_serialization_callbacks.h"

#include "GameEntity.hpp"
#include "Vec2f.h"

#include "components/transform_component.h"

#include "core/buffer.h"

void SerializeForOwner( const GameEntity& entity, NetLib::Buffer& buffer )
{
	const TransformComponent& transform = entity.GetComponent< TransformComponent >();
	const Vec2f position = transform.GetPosition();
	buffer.WriteFloat( position.X() );
	buffer.WriteFloat( position.Y() );
	buffer.WriteFloat( transform.GetRotationAngle() );
}

void SerializeForNonOwner( const GameEntity& entity, NetLib::Buffer& buffer )
{
	const TransformComponent& transform = entity.GetComponent< TransformComponent >();
	const Vec2f position = transform.GetPosition();
	buffer.WriteFloat( position.X() );
	buffer.WriteFloat( position.Y() );
	buffer.WriteFloat( transform.GetRotationAngle() );
}

void DeserializeForOwner( GameEntity& entity, NetLib::Buffer& buffer )
{
	TransformComponent& transform = entity.GetComponent< TransformComponent >();
	Vec2f position;
	position.X( buffer.ReadFloat() );
	position.Y( buffer.ReadFloat() );

	transform.SetPosition( position );

	const float32 rotation_angle = buffer.ReadFloat();
	transform.SetRotationAngle( rotation_angle );
}
