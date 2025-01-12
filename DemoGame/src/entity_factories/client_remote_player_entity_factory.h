#pragma once
#include "entity_factories/i_entity_factory.h"

class ClientRemotePlayerEntityFactory : public IEntityFactory
{
	public:
		ClientRemotePlayerEntityFactory();

		void Create( GameEntity& entity, const BaseEntityConfiguration* configuration ) override;
		void Destroy( GameEntity& entity ) override;
};
