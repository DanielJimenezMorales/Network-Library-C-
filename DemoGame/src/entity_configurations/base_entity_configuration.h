#pragma once
#include <string>

#include "Vec2f.h"
#include "numeric_types.h"

struct BaseEntityConfiguration
{
		std::string name;
		Vec2f position;
		float32 lookAt;
};
