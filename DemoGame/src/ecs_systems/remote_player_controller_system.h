#pragma once
#include "ecs/i_simple_system.h"

class RemotePlayerControllerSystem : public ECS::ISimpleSystem
{
	public:
		RemotePlayerControllerSystem();

		void Execute( ECS::EntityContainer& entity_container, float32 elapsed_time ) override;
};
