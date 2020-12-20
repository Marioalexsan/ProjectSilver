#ifndef GAME_HEADER
#define GAME_HEADER

#include "AssetManager.h"
#include "AudioEngine.h"
#include "GraphicsEngine.h"
#include "InputHandler.h"
#include "Animation.h"

namespace Game {
	class GameMaster {
	private:
		bool gameRunning;

		map<string, Animation> animationLibrary;

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

		void AddAnimation(const string& ID, const Animation& animation);

		void SetAnimationInfo(const string& ID, Animation::Info info);

		inline const map<string, Animation>& GetAnimationLibrary() {
			return animationLibrary;
		}
	};
}

#endif
