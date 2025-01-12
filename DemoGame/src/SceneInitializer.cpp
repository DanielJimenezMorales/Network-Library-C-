#include "SceneInitializer.h"
#include "Scene.h"
#include "GameEntity.hpp"
#include "core/client.h"
#include "core/server.h"
#include "core/initializer.h"
#include "NetworkEntityFactory.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "InputActionIdsConfiguration.h"
#include "InputHandler.h"
#include "ITextureLoader.h"
#include "InputStateFactory.h"
#include "ServiceLocator.h"
#include "CircleBounds2D.h"

#include "ecs/system_coordinator.h"

#include "components/transform_component.h"
#include "components/virtual_mouse_component.h"
#include "components/sprite_renderer_component.h"
#include "components/crosshair_component.h"
#include "components/camera_component.h"
#include "components/input_component.h"
#include "components/collider_2d_component.h"
#include "components/gizmo_renderer_component.h"

#include "global_components/network_peer_global_component.h"

#include "ecs_systems/server_player_controller_system.h"
#include "ecs_systems/client_player_controller_system.h"
#include "ecs_systems/remote_player_controller_system.h"
#include "ecs_systems/crosshair_follow_mouse_system.h"
#include "ecs_systems/sprite_renderer_system.h"
#include "ecs_systems/gizmo_renderer_system.h"
#include "ecs_systems/virtual_mouse_system.h"
#include "ecs_systems/pre_tick_network_system.h"
#include "ecs_systems/pos_tick_network_system.h"
#include "ecs_systems/collision_detection_system.h"

#include "entity_factories/client_local_player_entity_factory.h"
#include "entity_factories/client_remote_player_entity_factory.h"
#include "entity_factories/server_player_entity_factory.h"

void SceneInitializer::InitializeScene( Scene& scene, NetLib::PeerType networkPeerType, InputHandler& inputHandler,
                                        SDL_Renderer* renderer ) const
{
	// Inputs
	KeyboardController* keyboard = new KeyboardController();
	InputButton button( JUMP_BUTTON, SDLK_q );
	keyboard->AddButtonMap( button );
	InputAxis axis( HORIZONTAL_AXIS, SDLK_d, SDLK_a );
	keyboard->AddAxisMap( axis );
	InputAxis axis2( VERTICAL_AXIS, SDLK_w, SDLK_s );
	keyboard->AddAxisMap( axis2 );
	inputHandler.AddController( keyboard );

	MouseController* mouse = new MouseController();
	InputButton mouseButton( MOUSE_LEFT_CLICK_BUTTON, SDL_BUTTON_LEFT );
	mouse->AddButtonMap( mouseButton );
	inputHandler.AddCursor( mouse );

	// Populate entities
	GameEntity mainCameraEntity = scene.CreateGameEntity();
	// TODO Do not hardcode width and height values
	mainCameraEntity.AddComponent< CameraComponent >( Vec2f( 0.f, 0.f ), 512, 512 );

	GameEntity inputsEntity = scene.CreateGameEntity();
	inputsEntity.AddComponent< InputComponent >( keyboard, mouse );

	NetworkPeerGlobalComponent& networkPeerComponent = scene.AddGlobalComponent< NetworkPeerGlobalComponent >();
	NetLib::Peer* networkPeer;
	if ( networkPeerType == NetLib::PeerType::SERVER )
	{
		networkPeer = new NetLib::Server( 2 );
	}
	else if ( networkPeerType == NetLib::PeerType::CLIENT )
	{
		networkPeer = new NetLib::Client( 5 );
	}

	// TODO Make this initializer internal when calling to start
	NetLib::Initializer::Initialize();
	NetworkEntityFactory* networkEntityFactory = new NetworkEntityFactory();
	networkEntityFactory->SetScene( &scene );
	networkEntityFactory->SetPeerType( networkPeerType );
	networkPeer->RegisterNetworkEntityFactory( networkEntityFactory );
	networkPeerComponent.peer = networkPeer;

	if ( networkPeer->GetPeerType() == NetLib::PeerType::SERVER )
	{
		// Entity factories registration
		ServerPlayerEntityFactory* player_entity_factory = new ServerPlayerEntityFactory();
		scene.RegisterEntityFactory( "PLAYER", player_entity_factory );

		InputStateFactory* inputStateFactory = new InputStateFactory();
		networkPeerComponent.GetPeerAsServer()->RegisterInputStateFactory( inputStateFactory );
		networkPeerComponent.inputStateFactory = inputStateFactory;
		networkPeerComponent.TrackOnRemotePeerConnect();

		// Add dummy collider entity
		GameEntity colliderEntity = scene.CreateGameEntity();
		TransformComponent& colliderEntityTransform = colliderEntity.GetComponent< TransformComponent >();
		colliderEntityTransform.SetPosition( Vec2f( 10.f, 10.f ) );
		ServiceLocator& serviceLocator = ServiceLocator::GetInstance();
		ITextureLoader& textureLoader = serviceLocator.GetTextureLoader();
		Texture* texture2 = textureLoader.LoadTexture( "sprites/PlayerSprites/PlayerHead.png" );
		colliderEntity.AddComponent< SpriteRendererComponent >( texture2 );

		CircleBounds2D* circleBounds2D = new CircleBounds2D( 5.f );
		colliderEntity.AddComponent< Collider2DComponent >( circleBounds2D, false, CollisionResponseType::Static );

		Gizmo* gizmo = circleBounds2D->GetGizmo();
		colliderEntity.AddComponent< GizmoRendererComponent >( gizmo );
	}

	if ( networkPeer->GetPeerType() == NetLib::PeerType::CLIENT )
	{
		// Entity factories registration
		ClientLocalPlayerEntityFactory* local_player_entity_factory = new ClientLocalPlayerEntityFactory();
		scene.RegisterEntityFactory( "LOCAL_PLAYER", local_player_entity_factory );

		ClientRemotePlayerEntityFactory* remote_player_entity_factory = new ClientRemotePlayerEntityFactory();
		scene.RegisterEntityFactory( "REMOTE_PLAYER", remote_player_entity_factory );

		// Add virtual mouse
		GameEntity virtualMouse = scene.CreateGameEntity();
		virtualMouse.AddComponent< VirtualMouseComponent >();

		// Add virtual mouse system
		ECS::SystemCoordinator* virtual_mouse_system_coordinator =
		    new ECS::SystemCoordinator( ECS::ExecutionStage::UPDATE );
		virtual_mouse_system_coordinator->AddSystemToTail( new VirtualMouseSystem() );
		scene.AddSystem( virtual_mouse_system_coordinator );

		// Add crosshair if being a client
		GameEntity crosshairEntity = scene.CreateGameEntity();

		ServiceLocator& serviceLocator = ServiceLocator::GetInstance();
		ITextureLoader& textureLoader = serviceLocator.GetTextureLoader();
		Texture* texture = textureLoader.LoadTexture( "sprites/Crosshair/crosshair.png" );

		crosshairEntity.AddComponent< SpriteRendererComponent >( texture );
		crosshairEntity.AddComponent< CrosshairComponent >();

		// Add crosshair follow mouse system
		ECS::SystemCoordinator* crosshair_follow_mouse_system_coordinator =
		    new ECS::SystemCoordinator( ECS::ExecutionStage::UPDATE );
		crosshair_follow_mouse_system_coordinator->AddSystemToTail( new CrosshairFollowMouseSystem() );
		scene.AddSystem( crosshair_follow_mouse_system_coordinator );
	}

	// Populate systems
	// TODO Create a system storage in order to be able to free them at the end
	if ( networkPeerType == NetLib::PeerType::SERVER )
	{
		/////////////////////
		// PRE TICK SYSTEMS
		/////////////////////

		// Add Server-side collision detection system
		ECS::SystemCoordinator* collision_detection_system_coordinator =
		    new ECS::SystemCoordinator( ECS::ExecutionStage::PRETICK );
		collision_detection_system_coordinator->AddSystemToTail( new CollisionDetectionSystem() );
		scene.AddSystem( collision_detection_system_coordinator );

		//////////////////
		// TICK SYSTEMS
		//////////////////

		// Add Server-side player controller system
		ECS::SystemCoordinator* server_player_controller_system_coordinator =
		    new ECS::SystemCoordinator( ECS::ExecutionStage::TICK );
		server_player_controller_system_coordinator->AddSystemToTail( new ServerPlayerControllerSystem() );
		scene.AddSystem( server_player_controller_system_coordinator );
	}
	else if ( networkPeerType == NetLib::PeerType::CLIENT )
	{
		//////////////////
		// TICK SYSTEMS
		//////////////////

		// Add Client-side player controller system
		ECS::SystemCoordinator* client_player_controller_system_coordinator =
		    new ECS::SystemCoordinator( ECS::ExecutionStage::TICK );
		client_player_controller_system_coordinator->AddSystemToTail( new ClientPlayerControllerSystem() );
		scene.AddSystem( client_player_controller_system_coordinator );

		// Add Client-side remote player controller system
		ECS::SystemCoordinator* client_remote_player_controller_system_coordinator =
		    new ECS::SystemCoordinator( ECS::ExecutionStage::TICK );
		client_player_controller_system_coordinator->AddSystemToTail( new RemotePlayerControllerSystem() );
		scene.AddSystem( client_remote_player_controller_system_coordinator );
	}

	/////////////////////
	// PRE TICK SYSTEMS
	/////////////////////

	// Add pre-tick network system
	ECS::SystemCoordinator* pre_tick_network_system_coordinator =
	    new ECS::SystemCoordinator( ECS::ExecutionStage::PRETICK );
	pre_tick_network_system_coordinator->AddSystemToTail( new PreTickNetworkSystem() );
	scene.AddSystem( pre_tick_network_system_coordinator );

	/////////////////////
	// POS TICK SYSTEMS
	/////////////////////

	// Add pos-tick network system
	ECS::SystemCoordinator* pos_tick_network_system_coordinator =
	    new ECS::SystemCoordinator( ECS::ExecutionStage::POSTICK );
	pos_tick_network_system_coordinator->AddSystemToTail( new PosTickNetworkSystem() );
	scene.AddSystem( pos_tick_network_system_coordinator );

	//////////////////
	// RENDER SYSTEMS
	//////////////////

	ECS::SystemCoordinator* render_system_coordinator = new ECS::SystemCoordinator( ECS::ExecutionStage::RENDER );
	render_system_coordinator->AddSystemToTail( new SpriteRendererSystem( renderer ) );

	GizmoRendererSystem* gizmo_renderer_system = new GizmoRendererSystem( renderer );
	render_system_coordinator->AddSystemToTail( gizmo_renderer_system );
	scene.AddSystem( render_system_coordinator );
	scene.SubscribeToOnEntityCreate( std::bind( &GizmoRendererSystem::AllocateGizmoRendererComponentIfHasCollider,
	                                            gizmo_renderer_system, std::placeholders::_1 ) );
	scene.SubscribeToOnEntityDestroy( std::bind( &GizmoRendererSystem::DeallocateGizmoRendererComponentIfHasCollider,
	                                             gizmo_renderer_system, std::placeholders::_1 ) );
}
