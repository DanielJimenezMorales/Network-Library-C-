#pragma once
#include "numeric_types.h"

class GameEntity;

namespace ECS
{
	class EntityContainer;

	class ISimpleSystem
	{
		public:
			ISimpleSystem() {}
			virtual ~ISimpleSystem() {}

			virtual void Execute( EntityContainer& entity_container, float32 elapsed_time ) = 0;
	};
}
