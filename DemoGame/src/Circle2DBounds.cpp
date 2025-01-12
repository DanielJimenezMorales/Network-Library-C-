#include "CircleBounds2D.h"

#include "Circle2DGizmo.h"

#include "components/transform_component.h"

// TODO Change name to this file
void CircleBounds2D::GetAxes( const TransformComponent& transform, std::vector< Vec2f >& outAxes ) const
{
	// Circle axes are calculated apart since it is not a default convex polygon but a curve shape
}

void CircleBounds2D::ProjectAxis( const TransformComponent& transform, const Vec2f& axis, float& outMin,
                                  float& outMax ) const
{
	/*In order to get the min and max projection points of the circle, we have to do the following steps :
	//1. Find the circle's center = C
	//2. Find the axis = A
	//3. Project the circle's center onto the axis. This is calculated by taking the dot product of the center with the
	axis:
	//Projection of the center = C' = dotProduct(C, A) = (Cx * Ax) + (Cy * Ay)
	//4. Find the circle's min and max projections. This is calculated using the following formulas:
	//Min projection = CenterProjection - Radius
	//Max projection = CenterProjection + Radius
	*/

	const Vec2f center = transform.GetPosition();
	const float32 centerProjection = ( center.X() * axis.X() ) + ( center.Y() * axis.Y() );
	outMin = centerProjection - _radius;
	outMax = centerProjection + _radius;
}

Vec2f CircleBounds2D::GetClosestVertex( const TransformComponent& transform, const Vec2f& inputPoint ) const
{
	return Vec2f();
}

float32 CircleBounds2D::GetMinX( const TransformComponent& transform ) const
{
	return transform.GetPosition().X() - _radius;
}

float32 CircleBounds2D::GetMaxX( const TransformComponent& transform ) const
{
	return transform.GetPosition().X() + _radius;
}

Gizmo* CircleBounds2D::GetGizmo() const
{
	return new CircleGizmo( _radius );
}
