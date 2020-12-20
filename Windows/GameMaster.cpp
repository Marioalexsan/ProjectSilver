#include "PCHeader.h"

#include "GameMaster.h"

namespace Game {
	GameMaster::GameMaster():
		gameRunning(true)
	{
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

	void GameMaster::AddAnimation(const string& ID, const Animation& animation) {
		if (animationLibrary.find(ID) == animationLibrary.end()) {
			animationLibrary[ID] = animation;
		}
	}

	void GameMaster::SetAnimationInfo(const string& ID, Animation::Info info) {
		if (animationLibrary.find(ID) == animationLibrary.end()) {
			return;
		}
		animationLibrary[ID].SetInfo(info);
	}
}