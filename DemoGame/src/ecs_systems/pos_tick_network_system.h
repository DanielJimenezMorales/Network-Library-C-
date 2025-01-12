#pragma once
#include "ecs/i_simple_system.h"

class PosTickNetworkSystem : public ECS::ISimpleSystem
{
	public:
		PosTickNetworkSystem();

		void Execute( ECS::EntityContainer& entity_container, float32 elapsed_time ) override;
};
