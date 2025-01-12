#pragma once
#include <cstdint>

struct NetworkEntityComponent
{
	NetworkEntityComponent(uint32 entityId, uint32 controlledByPeerId) : networkEntityId(entityId), controlledByPeerId(controlledByPeerId)
	{
	}

	uint32 networkEntityId;
	uint32 controlledByPeerId;
};
