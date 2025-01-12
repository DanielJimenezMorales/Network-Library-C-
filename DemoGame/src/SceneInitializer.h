#pragma once
#include "core/peer.h"

#include <SDL_image.h>

class Scene;
class InputHandler;

class SceneInitializer
{
	public:
		void InitializeScene( Scene& scene, NetLib::PeerType networkPeerType, InputHandler& inputHandler,
		                      SDL_Renderer* renderer ) const;
};
