#pragma once
#include "Peer.h"

class Scene;
class InputHandler;

class SceneInitializer
{
public:
	void InitializeScene(Scene& scene, NetLib::PeerType networkPeerType, InputHandler& inputHandler) const;
};