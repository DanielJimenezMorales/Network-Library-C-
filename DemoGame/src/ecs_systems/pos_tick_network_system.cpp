#include "pos_tick_network_system.h"

#include "GameEntity.hpp"

#include "global_components/network_peer_global_component.h"

PosTickNetworkSystem::PosTickNetworkSystem()
    : ECS::ISimpleSystem()
{
}

void PosTickNetworkSystem::Execute( ECS::EntityContainer& entity_container, float32 elapsed_time )
{
	NetworkPeerGlobalComponent& networkPeerComponent =
	    entity_container.GetGlobalComponent< NetworkPeerGlobalComponent >();
	networkPeerComponent.peer->Tick( elapsed_time );
}
