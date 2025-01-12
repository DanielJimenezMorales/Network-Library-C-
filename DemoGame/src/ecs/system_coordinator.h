#pragma once
#include "numeric_types.h"

#include <vector>

namespace ECS
{
	class ISimpleSystem;
	class EntityContainer;

	enum class ExecutionStage : uint8
	{
		UPDATE,
		PRETICK,
		TICK,
		POSTICK,
		RENDER
	};

	class SystemCoordinator
	{
		public:
			SystemCoordinator( ExecutionStage stage );

			ExecutionStage GetStage() const;

			// Adds a system to the tail of the vector. The tail is the lowest priority section
			void AddSystemToTail( ISimpleSystem* system );
			// Executes all systems in order, from highest to lowest priority
			void Execute( EntityContainer& entity_container, float elapsed_time );

		private:
			ExecutionStage _stage;
			std::vector< ISimpleSystem* > _systems;
	};
} // namespace ECS
