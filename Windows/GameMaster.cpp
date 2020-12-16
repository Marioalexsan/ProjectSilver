#include "PCHeader.h"

#include "GameMaster.h"

namespace Game {
	GameMaster::GameMaster():
		gameRunning(true)
	{
		Audio.SetContentManager(&Assets);
	}

	void GameMaster::Update(bool skipGraphicsFrame = false) {
		if (!gameRunning) {
			return;
		}
	}

	void GameMaster::Stop() {
		gameRunning = false;
	}

	bool GameMaster::IsGameRunning() {
		return gameRunning;
	}
}