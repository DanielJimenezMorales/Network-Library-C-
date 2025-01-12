#pragma once

#include <vector>

class GameEntity;

namespace ECS
{
	class EntityContainer;

	class IFilter
	{
		public:
			IFilter() {}
			virtual ~IFilter() {}

			virtual std::vector< GameEntity > Apply( EntityContainer& entity_container ) const = 0;
	};
}
