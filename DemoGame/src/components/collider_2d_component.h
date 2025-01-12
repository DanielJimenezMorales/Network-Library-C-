#pragma once
#include "Vec2f.h"
#include "Bounds2D.h"
#include <cstdint>
#include <vector>

struct TransformComponent;
class Gizmo;

enum class CollisionResponseType : uint8
{
	Static = 0,
	Dynamic = 1
};

struct Collider2DComponent
{
	public:
		Collider2DComponent( Bounds2D* bounds, bool isTrigger, CollisionResponseType responseType );
		Collider2DComponent( const Collider2DComponent& ) = delete;
		Collider2DComponent( Collider2DComponent&& other ) noexcept;

		~Collider2DComponent();

		Collider2DComponent& operator=( const Collider2DComponent& ) = delete;
		Collider2DComponent& operator=( Collider2DComponent&& other ) noexcept;

		CollisionShapeType GetShapeType() const { return _bounds->GetShapeType(); }
		void GetAxes( const TransformComponent& transform, std::vector< Vec2f >& outAxes ) const;
		void ProjectAxis( const TransformComponent& transform, const Vec2f& axis, float& outMin, float& outMax ) const;
		Vec2f GetClosestVertex( const TransformComponent& transform, const Vec2f& inputPoint ) const;

		float32 GetMinX( const TransformComponent& transform ) const;
		float32 GetMaxX( const TransformComponent& transform ) const;

		Gizmo* GetGizmo() const;

		CollisionResponseType GetCollisionResponse() const { return _collisionResponseType; }
		bool IsTrigger() const { return _isTrigger; }

	private:
		Bounds2D* _bounds;
		bool _isTrigger;
		CollisionResponseType _collisionResponseType;
};
