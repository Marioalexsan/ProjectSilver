#ifndef GAME_HEADER
#define GAME_HEADER

#include "AssetManager.h"
#include "AudioEngine.h"
#include "GraphicsEngine.h"
#include "InputHandler.h"

namespace Game {
	class GameMaster {
	private:
		bool gameRunning;
	public:
		const double fixedTimeStep = 16.67;
		const int maxFrameRate = 60;

		Game::AudioEngine Audio;
		Game::GraphicsEngine Graphics;
		Game::AssetManager Assets;
		Game::InputHandler Input;

		GameMaster();

		void Update(bool skipGraphicsFrame);
		void Stop();
		bool IsGameRunning();
	};
}

#endif
