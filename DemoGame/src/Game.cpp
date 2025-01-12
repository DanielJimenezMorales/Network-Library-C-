#include "Game.h"

#include "logger.h"

#include "core/peer.h"
#include "core/initializer.h"

#include "SceneInitializer.h"
#include "ServiceLocator.h"
#include "TextureLoader.h"

bool Game::Init()
{
	LOG_INFO( "Select:" );
	LOG_INFO( "Server: 0" );
	LOG_INFO( "Client: 1" );

	int32 clientOrServer;
	std::cin >> clientOrServer;

	int32 result = InitSDL();
	if ( result != 0 )
	{
		LOG_ERROR( "Error while initializing SDL. Error code: %s", SDL_GetError() );
		return false;
	}

	result = CreateWindowAndRenderer();
	if ( result != 0 )
	{
		LOG_ERROR( "Error while initializing SDL window. Error code: %s", SDL_GetError() );
		return false;
	}

	result = SDL_SetRenderDrawColor( _renderer, 255, 0, 0, 255 );
	if ( result != 0 )
	{
		LOG_ERROR( "Error while setting SDL render draw color. Error code: %s", SDL_GetError() );
		return false;
	}

	result = SDL_SetRelativeMouseMode( SDL_TRUE );
	if ( result != 0 )
	{
		LOG_ERROR( "Error while setting SDL mouse to relative mode. Error code: %s", SDL_GetError() );
		return false;
	}

	_isRunning = true;

	ServiceLocator& serviceLocator = ServiceLocator::CreateInstance();
	TextureLoader* textureLoader = new TextureLoader();
	textureLoader->Init( _renderer );
	serviceLocator.RegisterTextureLoader( textureLoader );

	SceneInitializer sceneInitializer;

	NetLib::PeerType peerType = clientOrServer == 0 ? NetLib::PeerType::SERVER : NetLib::PeerType::CLIENT;
	sceneInitializer.InitializeScene( _activeScene, peerType, _inputHandler, _renderer );

	return true;
}

void Game::GameLoop()
{
	NetLib::TimeClock& timeClock = NetLib::TimeClock::GetInstance();
	float64 accumulator = 0.0;

	while ( _isRunning )
	{
		timeClock.UpdateLocalTime();
		accumulator += timeClock.GetElapsedTimeSeconds();

		HandleEvents();

		while ( accumulator >= FIXED_FRAME_TARGET_DURATION )
		{
			PreTick( FIXED_FRAME_TARGET_DURATION );
			Tick( FIXED_FRAME_TARGET_DURATION );
			PosTick( FIXED_FRAME_TARGET_DURATION );
			accumulator -= FIXED_FRAME_TARGET_DURATION;
		}

		Update( timeClock.GetElapsedTimeSeconds() );
		Render( timeClock.GetElapsedTimeSeconds() );
		EndOfFrame();
	}
}

void Game::HandleEvents()
{
	_inputHandler.PreHandleEvents();

	SDL_Event ev;
	while ( SDL_PollEvent( &ev ) )
	{
		if ( ev.type == SDL_QUIT )
		{
			_isRunning = false;
		}
		else
		{
			_inputHandler.HandleEvent( ev );
		}
	}

	_inputHandler.PostHandleEvents();
}

void Game::PreTick( float32 tickElapsedTime )
{
	_activeScene.PreTick( tickElapsedTime );
}

void Game::Tick( float32 tickElapsedTime )
{
	_activeScene.Tick( tickElapsedTime );
}

void Game::PosTick( float32 tickElapsedTime )
{
	_activeScene.PosTick( tickElapsedTime );
}

void Game::Update( float32 elapsedTime )
{
	_activeScene.Update( elapsedTime );
}

void Game::Render( float32 elapsed_time )
{
	SDL_RenderClear( _renderer );

	_activeScene.Render( elapsed_time );

	SDL_RenderPresent( _renderer );
}

void Game::EndOfFrame()
{
	_activeScene.EndOfFrame();
}

bool Game::Release()
{
	NetLib::Initializer::Finalize();

	ServiceLocator& serviceLocator = ServiceLocator::GetInstance();
	serviceLocator.UnregisterAll();
	ServiceLocator::DestroyInstance();

	SDL_DestroyRenderer( _renderer );
	SDL_DestroyWindow( _window );
	SDL_Quit();

	return true;
}

int32 Game::InitSDL()
{
	return SDL_Init( SDL_INIT_EVERYTHING );
}

int32 Game::CreateWindowAndRenderer()
{
	return SDL_CreateWindowAndRenderer( 512, 512, SDL_WINDOW_SHOWN, &_window, &_renderer );
}
