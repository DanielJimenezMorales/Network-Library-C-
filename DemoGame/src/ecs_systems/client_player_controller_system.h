#pragma once
#include "ecs/i_simple_system.h"

class ClientPlayerControllerSystem : public ECS::ISimpleSystem
{
	public:
		ClientPlayerControllerSystem();

		void Execute( std::vector< GameEntity >& entities, ECS::EntityContainer& entity_container,
		              float32 elapsed_time ) override;
};