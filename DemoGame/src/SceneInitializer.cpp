#include "SceneInitializer.h"
#include "Scene.h"
#include "ScriptSystem.h"
#include "NetworkSystem.h"
#include "GameEntity.hpp"
#include "NetworkPeerComponent.h"
#include "CurrentTickComponent.h"
#include "Client.h"
#include "Server.h"
#include "Initializer.h"
#include "NetworkEntityFactory.h"
#include "KeyboardController.h"
#include "InputActionIdsConfiguration.h"
#include "InputHandler.h"

void SceneInitializer::InitializeScene(Scene& scene, NetLib::PeerType networkPeerType, SDL_Renderer* renderer, InputHandler& inputHandler) const
{
    //TODO Decouple SDL renderer. I dont want this method a renderer parameter. Create a "Texture loader" or something like that and use it there. This class will only act as a
    // consumer asking for certain textures and or surfaces. Maybe based on an ID

    //TODO 
    //Inputs
    KeyboardController* keyboard = new KeyboardController();
    InputButton button(JUMP_BUTTON, SDLK_q);
    keyboard->AddButtonMap(button);
    InputAxis axis(HORIZONTAL_AXIS, SDLK_d, SDLK_a);
    keyboard->AddAxisMap(axis);
    InputAxis axis2(VERTICAL_AXIS, SDLK_s, SDLK_w);
    keyboard->AddAxisMap(axis2);
    inputHandler.AddController(keyboard);

	//Populate entities
    GameEntity currentTickEntity = scene.CreateGameEntity();
    currentTickEntity.AddComponent<CurrentTickComponent>();

    GameEntity networkPeerEntity = scene.CreateGameEntity();
    NetworkPeerComponent& networkPeerComponent = networkPeerEntity.AddComponent<NetworkPeerComponent>();
    NetLib::Peer* networkPeer;
    if (networkPeerType == NetLib::PeerType::ServerMode)
    {
        networkPeer = new NetLib::Server(2);
    }
    else if (networkPeerType == NetLib::PeerType::ClientMode)
    {
        networkPeer = new NetLib::Client(5);
    }

    //TODO Make this initializer internal when calling to start
    NetLib::Initializer::Initialize();
    NetworkEntityFactory* networkEntityFactory = new NetworkEntityFactory();
    networkEntityFactory->SetRenderer(renderer); //TODO this shouldn't need a renderer. Decouple it
    networkEntityFactory->SetScene(&scene);
    networkEntityFactory->SetPeerType(networkPeerType);
    networkEntityFactory->SetKeyboard(keyboard);
    networkPeer->RegisterNetworkEntityFactory(networkEntityFactory);
    networkPeerComponent.peer = networkPeer;

	//Populate systems
    //TODO Create a system storage in order to be able to free them at the end
    ScriptSystem* scriptSystem = new ScriptSystem();
    scene.AddUpdateSystem(scriptSystem);
    scene.AddTickSystem(scriptSystem);

    NetworkSystem* networkSystem = new NetworkSystem();
    scene.AddPreTickSystem(networkSystem);
    scene.AddPosTickSystem(networkSystem);
}
