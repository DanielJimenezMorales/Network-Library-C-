#pragma once
#include "ecs/system_coordinator.h"

#include <unordered_map>
#include <vector>

namespace ECS
{
	class EntityContainer;

	/// <summary>
	/// <para>The systems handler acts as a storage of all different systems for each execution stage.</para>
	/// <para>IMPORTANT: The systems handler do NOT owne the different systems and will not be responsible for memory
	/// deallocation.</para>
	/// </summary>
	class SystemsHandler
	{
		public:
			SystemsHandler();

			void AddSystem( SystemCoordinator* system );

			void Clear();

			void TickStage( EntityContainer& entity_container, float elapsed_time, ExecutionStage stage );

		private:
			std::unordered_map< ExecutionStage, std::vector< SystemCoordinator* > > _systems;
	};
}
