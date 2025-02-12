#pragma once
#include "numeric_types.h"

namespace NetLib
{
	struct NetworkEntityCommunicationCallbacks;

	class INetworkEntityFactory
	{
		public:
			INetworkEntityFactory() {}
			virtual ~INetworkEntityFactory() {}

			virtual int32 CreateNetworkEntityObject(
			    uint32 networkEntityType, uint32 networkEntityId, uint32 controlledByPeerId, float32 posX, float32 posY,
			    NetLib::NetworkEntityCommunicationCallbacks& communication_callbacks ) = 0;
			virtual void DestroyNetworkEntityObject( uint32 gameEntity ) = 0;
	};
}
