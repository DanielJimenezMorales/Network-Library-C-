#include "collision_detection_system.h"
#include "GameEntity.hpp"
#include "CollisionUtils.h"
#include "Logger.h"

#include "components/collider_2d_component.h"
#include "components/transform_component.h"

#include "ecs/entity_container.h"

CollisionDetectionSystem::CollisionDetectionSystem()
    : ECS::ISimpleSystem()
{
}

bool ReturnMinLeft( const GameEntity& colliderEntityA, const GameEntity& colliderEntityB )
{
	const Collider2DComponent& colliderA = colliderEntityA.GetComponent< Collider2DComponent >();
	const TransformComponent& transformA = colliderEntityA.GetComponent< TransformComponent >();

	const Collider2DComponent& colliderB = colliderEntityB.GetComponent< Collider2DComponent >();
	const TransformComponent& transformB = colliderEntityB.GetComponent< TransformComponent >();

	return colliderA.GetMinX( transformA ) < colliderB.GetMinX( transformB );
}

void CollisionDetectionSystem::Execute( ECS::EntityContainer& entity_container, float32 elapsed_time )
{
	std::vector< GameEntity > entities =
	    entity_container.GetEntitiesOfBothTypes< Collider2DComponent, TransformComponent >();
	TickSweepAndPrune( entities );
}

void CollisionDetectionSystem::TickSweepAndPrune( std::vector< GameEntity >& collision_entities ) const
{
	// Sort by left in order to optimize the number of collision queries later.
	SortCollidersByLeft( collision_entities );

	for ( uint32 i = 0; i < collision_entities.size(); ++i )
	{
		// Get first object collider & transform components
		const Collider2DComponent& colliderA = collision_entities[ i ].GetComponent< Collider2DComponent >();
		TransformComponent& transformA = collision_entities[ i ].GetComponent< TransformComponent >();

		for ( uint32 j = i + 1; j < collision_entities.size(); ++j )
		{
			// Get second object collider & transform components
			const Collider2DComponent& colliderB = collision_entities[ j ].GetComponent< Collider2DComponent >();
			TransformComponent& transformB = collision_entities[ j ].GetComponent< TransformComponent >();

			// Check if these two objects have any collision possibilities. If not, don't check the first object anymore
			// since the colliders are ordered by left, it means the rest of colliders won't have any collision
			// possibilities either.
			if ( colliderA.GetMaxX( transformA ) < colliderB.GetMinX( transformB ) )
			{
				break;
			}

			// Check for collision and if success, get the MTV for collision response
			MinimumTranslationVector mtv;
			if ( AreTwoShapesCollidingUsingSAT( colliderA, transformA, colliderB, transformB, mtv ) )
			{
				if ( !colliderA.IsTrigger() && !colliderB.IsTrigger() )
				{
					ApplyCollisionResponse( colliderA, transformA, colliderB, transformB, mtv );
				}
			}
		}
	}
}

bool CollisionDetectionSystem::AreTwoShapesCollidingUsingSAT( const Collider2DComponent& collider1,
                                                              const TransformComponent& transform1,
                                                              const Collider2DComponent& collider2,
                                                              const TransformComponent& transform2,
                                                              MinimumTranslationVector& outMtv ) const
{
	std::vector< Vec2f > axesToCheck;
	GetAllAxes( collider1, transform1, collider2, transform2, axesToCheck );
	NormalizeAxes( axesToCheck );

	Vec2f smallestAxis;
	float32 smallestOverlapMagnitude = MAX_FLOAT32;
	auto cit = axesToCheck.cbegin();
	for ( ; cit != axesToCheck.cend(); ++cit )
	{
		float32 minCollider1, maxCollider1, minCollider2, maxCollider2 = 0.f;
		collider1.ProjectAxis( transform1, *cit, minCollider1, maxCollider1 );
		collider2.ProjectAxis( transform2, *cit, minCollider2, maxCollider2 );

		float32 projectionOverlapMagnitude =
		    GetProjectionsOverlapMagnitude( minCollider1, maxCollider1, minCollider2, maxCollider2 );
		if ( projectionOverlapMagnitude > 0.f )
		{
			if ( projectionOverlapMagnitude < smallestOverlapMagnitude )
			{
				smallestAxis = *cit;
				smallestOverlapMagnitude = projectionOverlapMagnitude;
			}
		}
	}

	outMtv.direction = smallestAxis;
	outMtv.magnitude = smallestOverlapMagnitude;
	return true;
}

void CollisionDetectionSystem::SortCollidersByLeft( std::vector< GameEntity >& collider_entities ) const
{
	std::sort( collider_entities.begin(), collider_entities.end(), ReturnMinLeft );
}

void CollisionDetectionSystem::GetAllAxes( const Collider2DComponent& collider1, const TransformComponent& transform1,
                                           const Collider2DComponent& collider2, const TransformComponent& transform2,
                                           std::vector< Vec2f >& outAxesVector ) const
{
	if ( collider1.GetShapeType() == CollisionShapeType::Convex &&
	     collider2.GetShapeType() == CollisionShapeType::Convex )
	{
		collider1.GetAxes( transform1, outAxesVector );
		collider2.GetAxes( transform2, outAxesVector );
	}
	else if ( collider1.GetShapeType() == CollisionShapeType::Convex &&
	          collider2.GetShapeType() == CollisionShapeType::Circle )
	{
		collider1.GetAxes( transform1, outAxesVector );
		const Vec2f centerToClosestVertexAxis = collider1.GetClosestVertex( transform1, transform2.GetPosition() );
		outAxesVector.push_back( centerToClosestVertexAxis );
	}
	else if ( collider1.GetShapeType() == CollisionShapeType::Circle &&
	          collider2.GetShapeType() == CollisionShapeType::Convex )
	{
		const Vec2f centerToClosestVertexAxis = collider2.GetClosestVertex( transform2, transform1.GetPosition() );
		outAxesVector.push_back( centerToClosestVertexAxis );
		collider2.GetAxes( transform2, outAxesVector );
	}
	else if ( collider1.GetShapeType() == CollisionShapeType::Circle &&
	          collider2.GetShapeType() == CollisionShapeType::Circle )
	{
		const Vec2f centerToCenterAxis = transform2.GetPosition() - transform1.GetPosition();
		outAxesVector.push_back( centerToCenterAxis );
	}
}

void CollisionDetectionSystem::NormalizeAxes( std::vector< Vec2f >& axesVector ) const
{
	auto it = axesVector.begin();
	for ( ; it != axesVector.end(); ++it )
	{
		it->Normalize();
	}
}

float32 CollisionDetectionSystem::GetProjectionsOverlapMagnitude( float32 minProjection1, float32 maxProjection1,
                                                                  float32 minProjection2, float32 maxProjection2 ) const
{
	if ( !DoProjectionsOverlap( minProjection1, maxProjection1, minProjection2, maxProjection2 ) )
	{
		return 0.f;
	}

	return std::min( maxProjection1, maxProjection2 ) - std::max( minProjection1, minProjection2 );
}

bool CollisionDetectionSystem::DoProjectionsOverlap( float32 minProjection1, float32 maxProjection1,
                                                     float32 minProjection2, float32 maxProjection2 ) const
{
	return ( maxProjection1 >= minProjection2 ) && ( maxProjection2 >= minProjection1 );
}

void CollisionDetectionSystem::ApplyCollisionResponse( const Collider2DComponent& collider1,
                                                       TransformComponent& transform1,
                                                       const Collider2DComponent& collider2,
                                                       TransformComponent& transform2,
                                                       const MinimumTranslationVector& mtv ) const
{
	Vec2f resultedTranslationVector = mtv.direction * mtv.magnitude;

	if ( collider1.GetCollisionResponse() == CollisionResponseType::Dynamic &&
	     collider2.GetCollisionResponse() == CollisionResponseType::Static )
	{
		transform1.SetPosition( transform1.GetPosition() - resultedTranslationVector );
	}
	else if ( collider1.GetCollisionResponse() == CollisionResponseType::Static &&
	          collider2.GetCollisionResponse() == CollisionResponseType::Dynamic )
	{
		transform2.SetPosition( transform2.GetPosition() + resultedTranslationVector );
	}
	else if ( collider1.GetCollisionResponse() == CollisionResponseType::Dynamic &&
	          collider2.GetCollisionResponse() == CollisionResponseType::Dynamic )
	{
		transform1.SetPosition( transform1.GetPosition() - ( resultedTranslationVector / 2.f ) );
		transform2.SetPosition( transform2.GetPosition() + ( resultedTranslationVector / 2.f ) );
	}
	else if ( collider1.GetCollisionResponse() == CollisionResponseType::Static &&
	          collider2.GetCollisionResponse() == CollisionResponseType::Static )
	{
		LOG_ERROR( "Collision response between two static colliders is not supported" );
	}
	else
	{
		LOG_ERROR( "Collision response not supported" );
	}
}
