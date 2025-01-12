#pragma once
#include <SDL.h>

#include "Scene.h"
#include "InputHandler.h"

const uint32 FIXED_FRAMES_PER_SECOND = 50;
const float32 FIXED_FRAME_TARGET_DURATION = 1.0f / FIXED_FRAMES_PER_SECOND;

class Game
{
	public:
		Game() = default;

		bool Init();
		void GameLoop();
		bool Release();

	private:
		int32 InitSDL();
		int32 CreateWindowAndRenderer();

		void HandleEvents();
		void PreTick( float32 tickElapsedTime );
		void Tick( float32 tickElapsedTime );
		void PosTick( float32 tickElapsedTime );
		void Update( float32 elapsedTime );
		void Render( float32 elapsed_time );
		void EndOfFrame();

		SDL_Window* _window;
		SDL_Renderer* _renderer;
		bool _isRunning;

		Scene _activeScene;
		// TODO The input handler should also be witin an ECS component
		InputHandler _inputHandler;
};