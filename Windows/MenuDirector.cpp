#include "PCHeader.h"
#include "MenuDirector.h"
#include "Globals.h"
#include "CollisionMaster.h"

namespace Game {
	MenuDirector::MenuDirector():
		button1Col(Game::Vector2(200, 400), 460, 150, Game::Collider::ColliderType::Static),
		button2Col(Game::Vector2(200, 600), 460, 150, Game::Collider::ColliderType::Static),
		button3Col(Game::Vector2(200, 800), 460, 150, Game::Collider::ColliderType::Static)
	{
		Globals::Game().Audio.SetMusicVolume(100.0);
		Globals::Game().Audio.PlayMusic("Menu");

		Globals::Graphics().CenterCameraOn(Vector2(960.0, 540.0));
		button1BG.SetTexture("Button");
		button1BG.SetPosition({ 200, 400 });
		button1BG.SetCenter({ 0, 0 });
		button1BG.SetLayer(Game::GraphicsEngine::CommonLayers::Background);
		button1BG.RegisterToGame();

		button1Text.SetFont("Huge");
		button1Text.SetText("Play");
		button1Text.SetPosition(Vector2(250, 450));
		button1Text.RegisterToGame();

		button2BG.SetTexture("Button");
		button2BG.SetPosition({ 200, 600 });
		button2BG.SetCenter({ 0, 0 });
		button2BG.SetLayer(Game::GraphicsEngine::CommonLayers::Background);
		button2BG.RegisterToGame();

		button2Text.SetFont("Huge");
		button2Text.SetText("Difficulty");
		button2Text.SetPosition(Vector2(220, 650));
		button2Text.RegisterToGame();

		button2Diff.SetFont("Huge");
		button2Diff.SetPosition(Vector2(760, 650));
		button2Diff.RegisterToGame();

		switch (Globals::Difficulty()) {
		case 0: {
			button2Diff.SetText("Easy");
			button2Diff.SetColor(Color::White);
		} break;
		case 1: {
			button2Diff.SetText("Normal");
			button2Diff.SetColor(Color::Orange);
		} break;
		case 2: {
			button2Diff.SetText("Hard");
			button2Diff.SetColor(Color::Red);
		} break;
		}

		button3BG.SetTexture("Button");
		button3BG.SetPosition({ 200, 800 });
		button3BG.SetCenter({ 0, 0 });
		button3BG.SetLayer(Game::GraphicsEngine::CommonLayers::Background);
		button3BG.RegisterToGame();

		button3Text.SetFont("Huge");
		button3Text.SetText("Quit");
		button3Text.SetPosition(Vector2(250, 850));
		button3Text.RegisterToGame();

		gameTitle.SetFont("Huge");
		gameTitle.SetText("Project Silver");
		gameTitle.SetPosition(Vector2(300, 50));
		gameTitle.RegisterToGame();
	}

	MenuDirector::~MenuDirector()
	{
		button1BG.UnregisterFromGame();
		button2BG.UnregisterFromGame();
		button3BG.UnregisterFromGame();

		button1Text.UnregisterFromGame();
		button2Text.UnregisterFromGame();
		button3Text.UnregisterFromGame();

		button2Diff.UnregisterFromGame();
		gameTitle.UnregisterFromGame();
	}

	void MenuDirector::Update()
	{
		if (Globals::Game().Input.IsButtonPressedThisFrame(InputHandler::ButtonCode::Left)) {
			auto pos = Globals::Game().Input.GetMousePosition();
			if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), button1Col)) {
				Globals::Game().InitLevel();
				toBeDestroyed = true;
				Globals::Game().Audio.PlaySound("ButtonClick");
			}
			else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), button2Col)) {
				int difficulty = (Globals::Difficulty() + 1) % 3;
				Globals::ChangeDifficulty((GameMaster::DifficultyLevel)difficulty);
				switch (difficulty) {
				case 0: {
					button2Diff.SetText("Easy");
					button2Diff.SetColor(Color::White);
				} break;
				case 1: {
					button2Diff.SetText("Normal");
					button2Diff.SetColor(Color::Orange);
				} break;
				case 2: {
					button2Diff.SetText("Hard");
					button2Diff.SetColor(Color::Red);
				} break;
				}
				Globals::Game().Audio.PlaySound("ButtonClick");
			}
			else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), button3Col)) {
				Globals::Game().Stop();
				Globals::Game().Audio.PlaySound("ButtonClick");
			}
		}
	}
}