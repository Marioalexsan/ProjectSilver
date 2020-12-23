#include "PCHeader.h"
#include "Globals.h"

namespace Game {
	GameMaster* Globals::theGame = nullptr;

	void Globals::SetTheGame(GameMaster& game) {
		theGame = &game;
	}

	void Globals::GameDeaded() {
		theGame = nullptr;
	}

	GameMaster& Globals::Game() {
		if (theGame == nullptr) {
			throw std::invalid_argument("Game was nullptr in Globals::Game()!");
		}
		return *theGame;
	}

	AudioEngine& Globals::Audio() {
		if (theGame == nullptr) {
			throw std::invalid_argument("Game was nullptr in Globals::Audio()!");
		}
		return theGame->Audio;
	}

	GraphicsEngine& Globals::Graphics() {
		if (theGame == nullptr) {
			throw std::invalid_argument("Game was nullptr in Globals::Graphics()!");
		}
		return theGame->Graphics;
	}

	AssetManager& Globals::Assets() {
		if (theGame == nullptr) {
			throw std::invalid_argument("Game was nullptr in Globals::Asset()!");
		}
		return theGame->Assets;
	}

	
}