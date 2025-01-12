#pragma once
#include "entity_factories/i_entity_factory.h"

class ServerPlayerEntityFactory : public IEntityFactory
{
	public:
		ServerPlayerEntityFactory();

		void Create( GameEntity& entity, const BaseEntityConfiguration* configuration ) override;
		void Destroy( GameEntity& entity ) override;
};
