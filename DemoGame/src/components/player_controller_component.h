#pragma once
#include "PlayerControllerConfiguration.h"

#include "replication/network_variable.hpp"
#include "replication/network_variable_changes_handler.h"

struct PlayerControllerComponent
{
	public:
		PlayerControllerComponent( NetLib::NetworkVariableChangesHandler* networkVariableChangesHandler,
		                           uint32 networkEntityId, const PlayerControllerConfiguration& configuration )
		    : networkEntityId( networkEntityId )
		    , posX( networkVariableChangesHandler, networkEntityId )
		    , posY( networkVariableChangesHandler, networkEntityId )
		    , rotationAngle( networkVariableChangesHandler, networkEntityId )
		    , configuration( configuration ){};

		const PlayerControllerConfiguration configuration;
		uint32 networkEntityId;
		NetLib::NetworkVariable< float > posX;
		NetLib::NetworkVariable< float > posY;

		NetLib::NetworkVariable< float > rotationAngle;
};
