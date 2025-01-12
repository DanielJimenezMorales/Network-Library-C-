#pragma once
#include "Bounds2D.h"

struct CircleBounds2D : public Bounds2D
{
	public:
		CircleBounds2D( float32 radius )
		    : Bounds2D( CollisionShapeType::Circle )
		    , _radius( radius )
		{
		}

		void GetAxes( const TransformComponent& transform, std::vector< Vec2f >& outAxes ) const override;
		void ProjectAxis( const TransformComponent& transform, const Vec2f& axis, float& outMin,
		                  float& outMax ) const override;
		Vec2f GetClosestVertex( const TransformComponent& transform, const Vec2f& inputPoint ) const override;

		float32 GetMinX( const TransformComponent& transform ) const override;
		float32 GetMaxX( const TransformComponent& transform ) const override;

		Gizmo* GetGizmo() const override;

	private:
		float32 _radius;
};
