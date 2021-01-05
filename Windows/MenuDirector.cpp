#include "PCHeader.h"
#include "MenuDirector.h"
#include "Globals.h"
#include "CollisionMaster.h"

namespace Game {
	MenuDirector::MenuDirector():
		button1Col(Game::Vector2(200, 400), 400, 150, Game::Collider::ColliderType::Static),
		button2Col(Game::Vector2(200, 700), 400, 150, Game::Collider::ColliderType::Static)
	{
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
		button2BG.SetPosition({ 200, 700 });
		button2BG.SetCenter({ 0, 0 });
		button2BG.SetLayer(Game::GraphicsEngine::CommonLayers::Background);
		button2BG.RegisterToGame();

		button2Text.SetFont("Huge");
		button2Text.SetText("Quit");
		button2Text.SetPosition(Vector2(250, 750));
		button2Text.RegisterToGame();

		gameTitle.SetFont("Huge");
		gameTitle.SetText("Project Silver");
		gameTitle.SetPosition(Vector2(300, 50));
		gameTitle.RegisterToGame();
	}

	MenuDirector::~MenuDirector()
	{
		button1BG.UnregisterFromGame();
		button2BG.UnregisterFromGame();

		button1Text.UnregisterFromGame();
		button2Text.UnregisterFromGame();
		gameTitle.UnregisterFromGame();
	}

	void MenuDirector::Update()
	{
		if (Globals::Game().Input.IsButtonPressedThisFrame(InputHandler::ButtonCode::Left)) {
			auto pos = Globals::Game().Input.GetMousePosition();
			if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), button1Col)) {
				Globals::Game().InitLevel();
				toBeDestroyed = true;
			}
			else if (CollisionMaster::PointCheckVSBox(Vector2(pos.first, pos.second), button2Col)) {
				Globals::Game().Stop();
			}
		}
	}
}