#include "CollisionUtils.h"
#include "MathUtils.h"
#include <cassert>

#include "components/transform_component.h"

bool CircleWithCircleOverlaps( const Vec2f& centerA, float32 radiusA, const Vec2f& centerB, float32 radiusB )
{
	const float32 squareRadiusSum = ( radiusA * radiusA ) + ( radiusB * radiusB );
	const float32 distanceBetweenCenters = GetNoSquareRootDistance( centerA, centerB );
	return distanceBetweenCenters < squareRadiusSum;
}

void SeparateCircleWithCircle( TransformComponent& transformA, float32 radiusA, TransformComponent& transformB,
                               float32 radiusB )
{
	const float32 radiusSum = radiusA + radiusB;
	const float32 distanceBetweenCenters = GetDistance( transformA.GetPosition(), transformB.GetPosition() );
	const float32 halfDistanceDifference = ( radiusSum - distanceBetweenCenters ) / 2.f;
	assert( halfDistanceDifference >= 0.f );

	TransformComponent& leftCircle =
	    ( transformA.GetPosition().X() <= transformB.GetPosition().X() ) ? transformA : transformB;
	TransformComponent& rightCircle =
	    ( transformA.GetPosition().X() <= transformB.GetPosition().X() ) ? transformB : transformA;

	Vec2f leftToRightDirection = rightCircle.GetPosition() - leftCircle.GetPosition();
	leftToRightDirection.Normalize();

	rightCircle.SetPosition( rightCircle.GetPosition() + ( leftToRightDirection * halfDistanceDifference ) );
	leftCircle.SetPosition( leftCircle.GetPosition() - ( leftToRightDirection * halfDistanceDifference ) );
}
