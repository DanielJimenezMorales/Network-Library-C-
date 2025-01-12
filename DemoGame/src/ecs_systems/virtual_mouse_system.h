#pragma once
#include "ecs/i_simple_system.h"

class VirtualMouseSystem : public ECS::ISimpleSystem
{
	public:
		void Execute( ECS::EntityContainer& entity_container, float32 elapsed_time ) override;
};
