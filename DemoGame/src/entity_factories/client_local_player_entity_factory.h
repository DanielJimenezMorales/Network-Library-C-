#pragma once
#include "entity_factories/i_entity_factory.h"

class ClientLocalPlayerEntityFactory : public IEntityFactory
{
	public:
		ClientLocalPlayerEntityFactory();

		void Create( GameEntity& entity, const BaseEntityConfiguration* configuration ) override;
		void Destroy( GameEntity& entity ) override;
};
