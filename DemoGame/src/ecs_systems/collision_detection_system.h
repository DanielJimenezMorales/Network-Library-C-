#pragma once
#include "ecs/i_simple_system.h"

#include "IPreTickSystem.h"
#include "Vec2f.h"
#include <vector>

struct Collider2DComponent;
struct TransformComponent;
class GameEntity;

struct MinimumTranslationVector
{
	public:
		MinimumTranslationVector()
		    : direction()
		    , magnitude( 0.f )
		{
		}

		MinimumTranslationVector( const Vec2f& direction, float32 magnitude )
		    : direction( direction )
		    , magnitude( magnitude )
		{
		}

		Vec2f direction;
		float32 magnitude;
};

bool ReturnMinLeft( const GameEntity& colliderEntityA, const GameEntity& colliderEntityB );

class CollisionDetectionSystem : public ECS::ISimpleSystem
{
	public:
		CollisionDetectionSystem();

		void Execute( ECS::EntityContainer& entity_container, float32 elapsed_time ) override;

	private:
		///////////////////////////////////
		// SWEEP & PRUNE RELATED METHODS //
		///////////////////////////////////

		/// <summary>
		/// <para>This method is a close approximation to the 1D Sweep & Prune collision detection algorithm.
		/// For more info see <see href="https://leanrada.com/notes/sweep-and-prune/">this link</see></para>
		/// </summary>
		void TickSweepAndPrune( std::vector< GameEntity >& collision_entities ) const;

		/// <summary>
		/// <para>Sort all potential colliders using X coordinate from left to right in order to be able to apply future
		/// optimizations.</para>
		/// </summary>
		void SortCollidersByLeft( std::vector< GameEntity >& collider_entities ) const;

		///////////////////////////////////////////////////////
		// SAT COLLISION DETECTION ALGORITHM RELATED METHODS //
		///////////////////////////////////////////////////////

		/// <summary>
		/// <para>This method applies SAT collision detection algorithm between two shapes. For more info see <see
		/// href="https://dyn4j.org/2010/01/sat/">this link</see></para> <para>NOTE: In case of collision, this method
		/// resturns true and mtv contains the magnitude and normalized direction of the same minimum translation vector
		/// (useful for collision response)</para>
		/// </summary>
		bool AreTwoShapesCollidingUsingSAT( const Collider2DComponent& collider1, const TransformComponent& transform1,
		                                    const Collider2DComponent& collider2, const TransformComponent& transform2,
		                                    MinimumTranslationVector& outMtv ) const;

		/// <summary>
		/// <para>Returns all axes neccessary for performing a SAT collision detection. Supported collisions are
		/// are:</para> <para>1) 2D convex shape VS 2D convex shape</para> <para>2) 2D convex shape VS 2D circle (or
		/// viceversa)</para> <para>3) 2D circle VS 2D circle</para>
		/// </summary>
		void GetAllAxes( const Collider2DComponent& collider1, const TransformComponent& transform1,
		                 const Collider2DComponent& collider2, const TransformComponent& transform2,
		                 std::vector< Vec2f >& outAxesVector ) const;
		void NormalizeAxes( std::vector< Vec2f >& axesVector ) const;

		/// <summary>
		/// <para>Calculates the overlap magnitude from the result of projecting both collision objects over an
		/// axis.</para> <para>NOTE: If projections don't overlap, this method returns 0, otherwise the overlap
		/// magnitude</para>
		/// </summary>
		/// <returns></returns>
		float32 GetProjectionsOverlapMagnitude( float32 minProjection1, float32 maxProjection1, float32 minProjection2,
		                                        float32 maxProjection2 ) const;

		/// <summary>
		/// <para>Checks if both collision object projections over the same axis are overlaping.</para>
		/// </summary>
		bool DoProjectionsOverlap( float32 minProjection1, float32 maxProjection1, float32 minProjection2,
		                           float32 maxProjection2 ) const;

		//////////////////////////////////////////////////
		// COLLISION RESPONSE ALGORITHM RELATED METHODS //
		//////////////////////////////////////////////////

		/// <summary>
		/// <para>Applies a collision response based on the mtv in order to separate both colliding shapes. This method
		/// supports the following types of collision responses:</para> <para>1) Dynamic collider VS Dynamic
		/// collider</para> <para>2) Dynamic collider VS Static collider (or viceversa)</para>
		/// </summary>
		void ApplyCollisionResponse( const Collider2DComponent& collider1, TransformComponent& transform1,
		                             const Collider2DComponent& collider2, TransformComponent& transform2,
		                             const MinimumTranslationVector& mtv ) const;
};
