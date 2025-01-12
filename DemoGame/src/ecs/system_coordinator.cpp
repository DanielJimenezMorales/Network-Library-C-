#include "system_coordinator.h"

#include "GameEntity.hpp"

#include "i_simple_system.h"

#include <cassert>

namespace ECS
{
	SystemCoordinator::SystemCoordinator( ExecutionStage stage )
	    : _stage( stage )
	    , _systemPairs()
	{
	}

	ExecutionStage SystemCoordinator::GetStage() const
	{
		return _stage;
	}

	void SystemCoordinator::AddSystemToTail( ISimpleSystem* system )
	{
		assert( system != nullptr );

		_systemPairs.push_back( system );
	}

	void SystemCoordinator::Execute( EntityContainer& entity_container, float elapsed_time )
	{
		auto system_pairs_it = _systemPairs.begin();
		for ( ; system_pairs_it != _systemPairs.end(); ++system_pairs_it )
		{
			// Execute system
			( *system_pairs_it )->Execute( entity_container, elapsed_time );
		}
	}
} // namespace ECS
