#pragma once
#include "Vec2f.h"

struct CameraComponent
{
	static constexpr uint32 PIXELS_PER_WORLD_UNIT = 10;

	CameraComponent(const Vec2f& position, int32 width, int32 height) : position(position), width(width), height(height)
	{
	}

	Vec2f ConvertFromWorldPositionToScreenPosition(Vec2f worldPosition) const
	{
		//Convert from world coordinates to uncentered screen coordinates
		worldPosition = (worldPosition - position) * PIXELS_PER_WORLD_UNIT;

		//This is due to SDL2 tracks positive Y downward, so I need to invert it. I want positive Y to go up by default
		worldPosition = Vec2f(worldPosition.X(), -worldPosition.Y());

		//Center position based on windows size (SDL2's center of coordinates is the top left position of the screen)
		worldPosition.AddToX(static_cast<float>(width / 2));
		worldPosition.AddToY(static_cast<float>(height / 2));
		return worldPosition;
	}

	Vec2f ConvertFromScreenPositionToWorldPosition(Vec2f screenPosition) const
	{
		screenPosition.AddToX(-static_cast<float>(width / 2));
		screenPosition.AddToY(-static_cast<float>(height / 2));

		//This is due to SDL2 tracks positive Y downward, so I need to invert it. I want positive Y to go up by default
		screenPosition = Vec2f(screenPosition.X(), -screenPosition.Y());

		screenPosition /= PIXELS_PER_WORLD_UNIT;
		screenPosition.AddToX(position.X());
		screenPosition.AddToY(position.Y());
		return screenPosition;
	}

	int32 GetScreenWidth() const { return width; }
	int32 GetScreenHeight() const { return height; }

	Vec2f position;
	int32 width;
	int32 height;
};