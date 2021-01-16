#include "PCHeader.h"
#include "MenuDirector.h"
#include "Globals.h"
#include "CollisionMaster.h"
#include "ConfigHandler.h"
#include "LogHandler.h"

namespace Game {
	MenuDirector::MenuDirector():
		currentVideoMode(0),
		currentState(MenuState::MainScreen)
	{

		// Some helper functions and / or classes could reduce this boilerplate down to size
		// TO DO: Create button drawable / action object hybrids

		Globals::Game().Audio.SetMusicVolume(100.0);
		Globals::Game().Audio.PlayMusic("Menu");

		Globals::Graphics().CenterCameraOn(Vector2(960.0, 540.0));

		// Colliders

		mainPlay.col = BoxCollider(Vector2(100, 400), 460, 150, Collider::ColliderType::Static);
		mainPlay.sprite.SetTexture("Button");
		mainPlay.sprite.SetLayer(GraphicsEngine::CommonLayers::Background);
		mainPlay.sprite.SetPosition(Vector2(100, 400));
		mainPlay.text.SetFont("Huge");
		mainPlay.text.SetText("Play");
		mainPlay.text.SetPosition(Vector2(100 + 230, 450));
		mainPlay.text.SetRenderType(BasicText::TextRenderType::ContinuousCentered);

		mainDiff.col = BoxCollider(Vector2(100, 600), 460, 150, Collider::ColliderType::Static);
		mainDiff.sprite.SetTexture("Button");
		mainDiff.sprite.SetLayer(GraphicsEngine::CommonLayers::Background);
		mainDiff.sprite.SetPosition(Vector2(100, 600));
		mainDiff.text.SetFont("Huge");
		mainDiff.text.SetText("Difficulty");
		mainDiff.text.SetPosition(Vector2(100 + 230, 650));
		mainDiff.text.SetRenderType(BasicText::TextRenderType::ContinuousCentered);
		
		mainDiffExtra.SetFont("Huge");
		mainDiffExtra.SetPosition(Vector2(660, 650));
		
		switch (Globals::Difficulty()) {
		case 0: {
			mainDiffExtra.SetText("Easy");
			mainDiffExtra.SetColor(Color::Aqua);
		} break;
		case 1: {
			mainDiffExtra.SetText("Normal");
			mainDiffExtra.SetColor(Color::White);
		} break;
		case 2: {
			mainDiffExtra.SetText("Hard");
			mainDiffExtra.SetColor(Color::Red);
		} break;
		}

		mainOptions.col = BoxCollider(Vector2(100, 800), 460, 150, Collider::ColliderType::Static);
		mainOptions.sprite.SetTexture("Button");
		mainOptions.sprite.SetLayer(GraphicsEngine::CommonLayers::Background);
		mainOptions.sprite.SetPosition(Vector2(100, 800));
		mainOptions.text.SetFont("Huge");
		mainOptions.text.SetText("Options");
		mainOptions.text.SetPosition(Vector2(100 + 230, 850));
		mainOptions.text.SetRenderType(BasicText::TextRenderType::ContinuousCentered);

		mainQuit.col = BoxCollider(Vector2(1300, 900), 460, 150, Collider::ColliderType::Static);
		mainQuit.sprite.SetTexture("Button");
		mainQuit.sprite.SetLayer(GraphicsEngine::CommonLayers::Background);
		mainQuit.sprite.SetPosition(Vector2(1300, 900));
		mainQuit.text.SetFont("Huge");
		mainQuit.text.SetText("Quit");
		mainQuit.text.SetPosition(Vector2(1300 + 230, 950));
		mainQuit.text.SetRenderType(BasicText::TextRenderType::ContinuousCentered);

		optionsRes.col = BoxCollider(Vector2(100, 400), 460, 150, Collider::ColliderType::Static);
		optionsRes.sprite.SetTexture("Button");
		optionsRes.sprite.SetLayer(GraphicsEngine::CommonLayers::Background);
		optionsRes.sprite.SetPosition(Vector2(100, 400));
		optionsRes.text.SetFont("Huge");
		optionsRes.text.SetText("Resolution");
		optionsRes.text.SetPosition(Vector2(100 + 230, 450));
		optionsRes.text.SetRenderType(BasicText::TextRenderType::ContinuousCentered);

		optionsResExtra.SetFont("Huge");
		optionsResExtra.SetPosition(Vector2(620, 450));

		optionsVideo.col = BoxCollider(Vector2(100, 600), 460, 150, Collider::ColliderType::Static);
		optionsVideo.sprite.SetTexture("Button");
		optionsVideo.sprite.SetLayer(GraphicsEngine::CommonLayers::Background);
		optionsVideo.sprite.SetPosition(Vector2(100, 600));
		optionsVideo.text.SetFont("Huge");
		optionsVideo.text.SetText("Video Mode");
		optionsVideo.text.SetPosition(Vector2(100 + 230, 650));
		optionsVideo.text.SetRenderType(BasicText::TextRenderType::ContinuousCentered);

		optionsVideoExtra.SetFont("Huge");
		optionsVideoExtra.SetPosition(Vector2(620, 650));

		optionsMain.col = BoxCollider(Vector2(100, 800), 460, 150, Collider::ColliderType::Static);
		optionsMain.sprite.SetTexture("Button");
		optionsMain.sprite.SetLayer(GraphicsEngine::CommonLayers::Background);
		optionsMain.sprite.SetPosition(Vector2(100, 800));
		optionsMain.text.SetFont("Huge");
		optionsMain.text.SetText("Go Back");
		optionsMain.text.SetPosition(Vector2(100 + 230, 850));
		optionsMain.text.SetRenderType(BasicText::TextRenderType::ContinuousCentered);

		// Music and Sound

		optionsMVol.subCol = BoxCollider(Vector2(1200, 400), 230, 150, Collider::ColliderType::Static);
		optionsMVol.addCol = BoxCollider(Vector2(1200 + 230, 400), 230, 150, Collider::ColliderType::Static);
		optionsMVol.sprite.SetTexture("ValueButton");
		optionsMVol.sprite.SetLayer(GraphicsEngine::CommonLayers::Background);
		optionsMVol.sprite.SetPosition(Vector2(1200, 400));
		optionsMVol.text.SetFont("Huge");
		optionsMVol.text.SetText("Music");
		optionsMVol.text.SetPosition(Vector2(1200 + 230, 440));
		optionsMVol.text.SetRenderType(BasicText::TextRenderType::ContinuousCentered);

		optionsMVolExtra.SetFont("Huge");
		optionsMVolExtra.SetPosition(Vector2(1200 + 230, 400 + 170));
		optionsMVolExtra.SetRenderType(BasicText::TextRenderType::ContinuousCentered);

		optionsSVol.subCol = BoxCollider(Vector2(1200, 750), 230, 150, Collider::ColliderType::Static);
		optionsSVol.addCol = BoxCollider(Vector2(1200 + 230, 750), 230, 150, Collider::ColliderType::Static);
		optionsSVol.sprite.SetTexture("ValueButton");
		optionsSVol.sprite.SetLayer(GraphicsEngine::CommonLayers::Background);
		optionsSVol.sprite.SetPosition(Vector2(1200, 750));
		optionsSVol.text.SetFont("Huge");
		optionsSVol.text.SetText("Sound");
		optionsSVol.text.SetPosition(Vector2(1200 + 230, 790));
		optionsSVol.text.SetRenderType(BasicText::TextRenderType::ContinuousCentered);

		optionsSVolExtra.SetFont("Huge");
		optionsSVolExtra.SetPosition(Vector2(1200 + 230, 750 + 170));
		optionsSVolExtra.SetRenderType(BasicText::TextRenderType::ContinuousCentered);

		gameTitle.SetFont("Huge");
		gameTitle.SetText("Project Silver");
		gameTitle.SetPosition(Vector2(960, 50));
		gameTitle.SetRenderType(BasicText::TextRenderType::ContinuousCentered);

		gameTitle.RegisterToGame();


		pair<int, int> res = ConfigHandler::GetConfigResolution();

		auto& resList = Globals::Graphics().Resolutions;

		currentVideoMode = Globals::Graphics().ResolutionCount - 1;
		for (int i = 0; i < Globals::Graphics().ResolutionCount; i++) {
			if (resList[i] == res) {
				currentVideoMode = i;
				break;
			}
		}

		optionsResExtra.SetText(std::to_string(resList[currentVideoMode].first) + "x" + std::to_string(resList[currentVideoMode].second));

		if (Globals::Graphics().IsFullscreen()) {
			optionsVideoExtra.SetText("Fullscreen");
		}
		else {
			optionsVideoExtra.SetText("Windowed");
		}

		optionsMVolExtra.SetText(std::to_string(Globals::Audio().GetUserMusicVolume()));
		optionsSVolExtra.SetText(std::to_string(Globals::Audio().GetUserSoundVolume()));
		
		StateEnter(currentState);
	}

	MenuDirector::~MenuDirector()
	{
		// Somewhat redundant
		StateExitCurrent();
		gameTitle.UnregisterFromGame();
		Game::LogHandler::Log("Exited Main Menu", Game::LogHandler::MessageType::Info);
	}

	void MenuDirector::StateEnter(MenuDirector::MenuState state) {
		currentState = state;
		switch (currentState) {
		case MenuDirector::MainScreen: {
			mainPlay.sprite.RegisterToGame();
			mainPlay.text.RegisterToGame();

			mainDiff.sprite.RegisterToGame();
			mainDiff.text.RegisterToGame();
			mainDiffExtra.RegisterToGame();

			mainOptions.sprite.RegisterToGame();
			mainOptions.text.RegisterToGame();

			mainQuit.sprite.RegisterToGame();
			mainQuit.text.RegisterToGame();
		} break;
		case MenuDirector::Options: {
			optionsRes.sprite.RegisterToGame();
			optionsRes.text.RegisterToGame();
			optionsResExtra.RegisterToGame();

			optionsVideo.sprite.RegisterToGame();
			optionsVideo.text.RegisterToGame();
			optionsVideoExtra.RegisterToGame();

			optionsMain.sprite.RegisterToGame();
			optionsMain.text.RegisterToGame();

			optionsMVol.sprite.RegisterToGame();
			optionsMVol.text.RegisterToGame();

			optionsMVolExtra.RegisterToGame();

			optionsSVol.sprite.RegisterToGame();
			optionsSVol.text.RegisterToGame();

			optionsSVolExtra.RegisterToGame();
		} break;
		}
	}

	void MenuDirector::StateExitCurrent() {
		switch (currentState) {
		case MenuDirector::MainScreen: {
			mainPlay.sprite.UnregisterFromGame();
			mainPlay.text.UnregisterFromGame();

			mainDiff.sprite.UnregisterFromGame();
			mainDiff.text.UnregisterFromGame();
			mainDiffExtra.UnregisterFromGame();

			mainOptions.sprite.UnregisterFromGame();
			mainOptions.text.UnregisterFromGame();

			mainQuit.sprite.UnregisterFromGame();
			mainQuit.text.UnregisterFromGame();
		} break;
		case MenuDirector::Options: {
			optionsRes.sprite.UnregisterFromGame();
			optionsRes.text.UnregisterFromGame();
			optionsResExtra.UnregisterFromGame();

			optionsVideo.sprite.UnregisterFromGame();
			optionsVideo.text.UnregisterFromGame();
			optionsVideoExtra.UnregisterFromGame();

			optionsMain.sprite.UnregisterFromGame();
			optionsMain.text.UnregisterFromGame();

			optionsMVol.sprite.UnregisterFromGame();
			optionsMVol.text.UnregisterFromGame();

			optionsMVolExtra.UnregisterFromGame();

			optionsSVol.sprite.UnregisterFromGame();
			optionsSVol.text.UnregisterFromGame();

			optionsSVolExtra.UnregisterFromGame();
		} break;
		}
	}

	void MenuDirector::Update()
	{
		if (Globals::Game().Input.IsButtonPressedThisFrame(InputHandler::ButtonCode::Right)) {
			SDL_RenderSetScale(Globals::Graphics().Renderer, 0.5f, 0.5f);
		}
		if (Globals::Game().Input.IsButtonPressedThisFrame(InputHandler::ButtonCode::Middle)) {
			SDL_RenderSetScale(Globals::Graphics().Renderer, 1.0f, 1.0f);
		}

		bool doClick = false;
		bool doVolume = false;
		switch (currentState) {
		case MenuState::MainScreen: {
			if (Globals::Game().Input.IsButtonPressedThisFrame(InputHandler::ButtonCode::Left)) {
				pair<int, int> pos = Globals::Game().Input.GetMousePosition();
				if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), mainPlay.col)) {
					Globals::Game().InitLevel();
					toBeDestroyed = true;
					doClick = true;
				}
				else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), mainDiff.col)) {
					int difficulty = (Globals::Difficulty() + 1) % 3;
					Globals::ChangeDifficulty((GameMaster::DifficultyLevel)difficulty);
					switch (difficulty) {
					case 0: {
						mainDiffExtra.SetText("Easy");
						mainDiffExtra.SetColor(Color::Aqua);
					} break;
					case 1: {
						mainDiffExtra.SetText("Normal");
						mainDiffExtra.SetColor(Color::White);
					} break;
					case 2: {
						mainDiffExtra.SetText("Hard");
						mainDiffExtra.SetColor(Color::Red);
					} break;
					}
					doClick = true;
				}
				else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), mainOptions.col)) {
					StateExitCurrent();
					StateEnter(MenuState::Options);
					doClick = true;
				}
				else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), mainQuit.col)) {
					Globals::Game().Stop();
					doClick = true;
				}
			}
		} break;
		case MenuState::Options: {
			if (Globals::Game().Input.IsButtonPressedThisFrame(InputHandler::ButtonCode::Left)) {
				pair<int, int> pos = Globals::Game().Input.GetMousePosition();
				if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), optionsRes.col)) {
					
					auto& resList = Globals::Graphics().Resolutions;
					currentVideoMode++;
					currentVideoMode %= Globals::Graphics().ResolutionCount;
					Globals::Graphics().SetDisplayMode({ resList[currentVideoMode].first, resList[currentVideoMode].second, Globals::Graphics().IsFullscreen() });
					optionsResExtra.SetText(std::to_string(resList[currentVideoMode].first) + "x" + std::to_string(resList[currentVideoMode].second));
					ConfigHandler::SetItem("resolution", std::to_string(resList[currentVideoMode].first) + "." + std::to_string(resList[currentVideoMode].second));
					doClick = true;
				}
				else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), optionsVideo.col)) {
					auto newVideoMode = Globals::Graphics().GetRenderDisplayMode();
					newVideoMode.fullscreen = !newVideoMode.fullscreen;
					Globals::Graphics().SetDisplayMode(newVideoMode);
					if (newVideoMode.fullscreen) {
						optionsVideoExtra.SetText("Fullscreen");
						ConfigHandler::SetItem("windowmode", "f");
					}
					else {
						optionsVideoExtra.SetText("Windowed");
						ConfigHandler::SetItem("windowmode", "w");
					}
					
					doClick = true;
				}
				else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), optionsMain.col)) {
					StateExitCurrent();
					StateEnter(MenuState::MainScreen);
					doClick = true;
				}
				// Music and Volume
				else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), optionsMVol.subCol)) {
					Globals::Audio().SetUserMusicVolume(Globals::Audio().GetUserMusicVolume() - 5.0);
					doClick = true;
					doVolume = true;
				}
				else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), optionsMVol.addCol)) {
					Globals::Audio().SetUserMusicVolume(Globals::Audio().GetUserMusicVolume() + 5.0);
					doClick = true;
					doVolume = true;
				}
				else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), optionsSVol.subCol)) {
					Globals::Audio().SetUserSoundVolume(Globals::Audio().GetUserSoundVolume() - 5.0);
					doClick = true;
					doVolume = true;
				}
				else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), optionsSVol.addCol)) {
					Globals::Audio().SetUserSoundVolume(Globals::Audio().GetUserSoundVolume() + 5.0);
					doClick = true;
					doVolume = true;
				}
			}
		} break;
		}
		
		if (doClick) {
			Globals::Game().Audio.PlaySound("ButtonClick");
		}
		if (doVolume) {
			optionsMVolExtra.SetText(std::to_string(Globals::Audio().GetUserMusicVolume()));
			optionsSVolExtra.SetText(std::to_string(Globals::Audio().GetUserSoundVolume()));
			ConfigHandler::SetItem("musicvolume", std::to_string(Globals::Audio().GetUserMusicVolume()));
			ConfigHandler::SetItem("soundvolume", std::to_string(Globals::Audio().GetUserSoundVolume()));
		}
	}
}