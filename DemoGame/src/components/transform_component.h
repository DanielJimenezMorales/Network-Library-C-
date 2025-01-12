#pragma once
#include "Vec2f.h"

/// <summary>
/// <para>Rotations will be represented using one float32 which will represent the rotation angles. This rotation is
/// anti-clockwise. A rotation angle of 0 degrees
///  will mean that the object is facing downward or in other words, its forward vector is (0, -1)</para>
/// </summary>
struct TransformComponent
{
	public:
		TransformComponent();
		TransformComponent( float32 x, float32 y );
		TransformComponent( const Vec2f& position, float32 rotation );

		TransformComponent( const TransformComponent& other ) = default;

		// Position
		Vec2f GetPosition() const { return _position; }
		void SetPosition( const Vec2f& newPosition ) { _position = newPosition; }

		// Rotation
		float32 GetRotationAngle() const { return _rotationAngle; }
		void SetRotationAngle( float32 newRotationAngle );
		void LookAt( const Vec2f& position );
		Vec2f GetForwardVector() const;

		// Scale
		Vec2f GetScale() const { return _scale; }
		void SetScale( const Vec2f& newScale ) { _scale = newScale; }

	private:
		Vec2f _position;

		float32 _rotationAngle;
		static constexpr Vec2f DEFAULT_FORWARD_VECTOR = Vec2f( 0.f, -1.f );

		Vec2f _scale;
};
