#pragma once
#include "TextureLoader.h"

struct SpriteRendererComponent
{
public:
	SpriteRendererComponent(Texture* tex) : texture(tex) {}

	Texture* texture;
};
