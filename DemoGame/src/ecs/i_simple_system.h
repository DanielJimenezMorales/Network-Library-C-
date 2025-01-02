#pragma once
#include "numeric_types.h"

class GameEntity;

namespace ECS
{
	class ISimpleSystem
	{
		public:
			ISimpleSystem() {}
			virtual ~ISimpleSystem() {}

			virtual void Execute( GameEntity& entity, float32 elapsed_time ) = 0;
	};
}
