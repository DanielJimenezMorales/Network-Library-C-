#pragma once
#include "INetworkEntityFactory.h"
#include "Scene.h"
#include "SDL.h"
#include "Peer.h"
#include "IInputController.h"

class NetworkEntityFactory : public NetLib::INetworkEntityFactory
{
public:
	NetworkEntityFactory() {}
	void SetRenderer(SDL_Renderer* renderer);
	void SetScene(Scene* scene);
	void SetKeyboard(IInputController* inputController);
	void SetPeerType(NetLib::PeerType peerType);
	int CreateNetworkEntityObject(uint32_t networkEntityType, uint32_t networkEntityId, float posX, float posY, NetLib::NetworkVariableChangesHandler* networkVariableChangeHandler) override;
	void DestroyNetworkEntityObject(uint32_t gameEntity) override;

private:
	Scene* _scene;
	SDL_Renderer* _renderer;
	IInputController* _inputController;

	NetLib::PeerType _peerType;
};
