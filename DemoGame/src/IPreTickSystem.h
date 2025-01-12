#pragma once
#include "numeric_types.h"

namespace ECS
{
	class EntityContainer;
}

class IPreTickSystem
{
	public:
		virtual void PreTick( ECS::EntityContainer& entityContainer, float32 elapsedTime ) const = 0;

	protected:
		IPreTickSystem() {}
};
