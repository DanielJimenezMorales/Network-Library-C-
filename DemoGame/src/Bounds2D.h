#pragma once
#include "Vec2f.h"
#include <vector>

struct TransformComponent;
class Gizmo;

enum class CollisionShapeType : uint8
{
	Convex = 0,
	Circle = 1,
};

class Bounds2D
{
	public:
		virtual ~Bounds2D() {}

		CollisionShapeType GetShapeType() const { return _shapeType; }
		virtual void GetAxes( const TransformComponent& transform, std::vector< Vec2f >& outAxes ) const = 0;
		virtual void ProjectAxis( const TransformComponent& transform, const Vec2f& axis, float& outMin,
		                          float& outMax ) const = 0;
		virtual Vec2f GetClosestVertex( const TransformComponent& transform, const Vec2f& inputPoint ) const = 0;

		virtual float32 GetMinX( const TransformComponent& transform ) const = 0;
		virtual float32 GetMaxX( const TransformComponent& transform ) const = 0;

		// Separate this one in a callback where it adds a GizmoComponent everytime an entity with a collider is spawned
		// and the global variable ENABLE_GIZMOS is enabled
		virtual Gizmo* GetGizmo() const = 0;

	protected:
		Bounds2D( CollisionShapeType shapeType )
		    : _shapeType( shapeType )
		{
		}

	private:
		CollisionShapeType _shapeType;
};
