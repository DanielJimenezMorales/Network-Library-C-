#include "systems_handler.h"

#include <cassert>

namespace ECS
{
	SystemsHandler::SystemsHandler()
	    : _systems()
	{
	}

	void SystemsHandler::AddSystem( SystemCoordinator* system )
	{
		assert( system != nullptr );

		const ExecutionStage stage = system->GetStage();
		auto systems_found = _systems.find( stage );

		if ( systems_found != _systems.end() )
		{
			systems_found->second.push_back( system );
		}
		else
		{
			_systems.insert( { stage, std::vector< SystemCoordinator* >( { system } ) } );
		}
	}

	void SystemsHandler::Clear()
	{
		_systems.clear();
	}

	void SystemsHandler::TickStage( EntityContainer& entity_container, float32 elapsed_time, ExecutionStage stage )
	{
		auto systems_found = _systems.find( stage );

		if ( systems_found != _systems.end() )
		{
			auto& stage_systems = systems_found->second;
			auto it = stage_systems.begin();
			for ( ; it != stage_systems.end(); ++it )
			{
				( *it )->Execute( entity_container, elapsed_time );
			}
		}
	}
} // namespace ECS
