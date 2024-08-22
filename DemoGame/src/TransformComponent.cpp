#include "TransformComponent.h"
#include <SDL_stdinc.h>
#include <cmath>
#include "Logger.h"

TransformComponent::TransformComponent() : _position(0.f, 0.f), _rotationAngle(0.f), _scale(1.f, 1.f)
{
}

TransformComponent::TransformComponent(float x, float y) : _position(x, y), _rotationAngle(0.f), _scale(1.f, 1.f)
{
}

void TransformComponent::LookAt(const Vec2f& position)
{
	Vec2f direction = position - _position;
	if (direction == Vec2f(0, 0))
	{
		return;
	}

	Vec2f forward = GetForwardVector();

	//FORMULA of angle between two vectors: Cos(angle) = dotProduct(V1, V1) / (Mag(V1) * Mag(V2)) --> angle = arcos(dotProduct(V1, V1) / (Mag(V1) * Mag(V2)))

	float dotProduct = (direction.X() * forward.X()) + (direction.Y() * forward.Y());
	float angleCosine = dotProduct / (direction.Magnitude() * forward.Magnitude());

	//Check it because due to floating point precision error, it could happen.
	if (angleCosine > 1.f)
	{
		angleCosine = 1.f;
	}
	else if (angleCosine < -1.f)
	{
		angleCosine = -1.f;
	}

	float angleInRadians = std::acosf(angleCosine);

	//Calculate rotation direction
	float crossProduct = (direction.X() * forward.Y()) - (forward.X() * direction.Y());
	if (crossProduct < 0.f)
	{
		angleInRadians = -angleInRadians;
	}

	float angleInDegrees = angleInRadians * (180.f / M_PI);

	SetRotationAngle(_rotationAngle + angleInDegrees);
}

Vec2f TransformComponent::GetForwardVector() const
{
	float angleInRadians = _rotationAngle * (M_PI / 180.f);

	//Since the rotation direction is anti-clockwise, we need to do the sin and cos of negative angle instead of just the positive angle.
	Vec2f forwardVector(std::sin(-angleInRadians), -std::cos(-angleInRadians));

	return forwardVector;
}

void TransformComponent::SetRotationAngle(float newRotationAngle)
{
	_rotationAngle = std::fmodf(newRotationAngle, 360.0f);
}