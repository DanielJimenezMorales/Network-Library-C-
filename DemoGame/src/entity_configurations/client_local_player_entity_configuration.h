#pragma once
#include "entity_configurations/base_entity_configuration.h"

namespace NetLib
{
	struct NetworkEntityCommunicationCallbacks;
}

struct ClientLocalPlayerEntityConfiguration : public BaseEntityConfiguration
{
		// NETWORK
		uint32 networkEntityId;
		uint32 controlledByPeerId;
		NetLib::NetworkEntityCommunicationCallbacks* communicationCallbacks;

		// MOVEMENT
		uint32 movementSpeed;
};
