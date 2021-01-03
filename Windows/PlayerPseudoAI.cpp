#include "PCHeader.h"
#include "PlayerPseudoAI.h"
#include "Globals.h"
#include "Actor.h"

// This is an AI in name only. In reality, this class interfaces with inputs
// From the game's engine

namespace Game {
    PlayerPseudoAI::PlayerPseudoAI():
        AI()
    {}

	void PlayerPseudoAI::Update() {
        
		if (entity == nullptr) {
			return;
		}

		auto& game = Globals::Game();
        using KeyCode = Game::InputHandler::KeyCode;
        using ButtonCode = Game::InputHandler::ButtonCode;

        if (game.Input.IsKeyDown(KeyCode::W)) {
            entity->Move({ 0, -10 });
        }
        if (game.Input.IsKeyDown(KeyCode::S)) {
            entity->Move({ 0, 10 });
        }
        if (game.Input.IsKeyDown(KeyCode::A)) {
            entity->Move({ -10, 0 });
        }
        if (game.Input.IsKeyDown(KeyCode::D)) {
            entity->Move({ 10, 0 });
        }

        if (game.Input.IsButtonPressedThisFrame(ButtonCode::Left)) {
            auto mouse = game.Input.GetMousePosition();
            auto results = game.CreateRayCastHitList(entity->GetCollider().GetPosition(), Game::Vector2(mouse.first, mouse.second) - Game::Vector2(960.0, 540.0) + entity->GetCollider().GetPosition());
            std::cout << "Hit list: " << endl;
            for (auto& elem : results) {
                std::cout << "Distance: " << elem.first << " ";
                using Game::BoxCollider;
                using Game::SphereCollider;
                BoxCollider* ABox = dynamic_cast<BoxCollider*>(elem.second);
                SphereCollider* ASphere = nullptr;
                if (ABox == nullptr) {
                    ASphere = dynamic_cast<SphereCollider*>(elem.second);
                }
                if (ABox) {
                    std::cout << "Box" << endl;
                }
                else if (ASphere) {
                    std::cout << "Sphere" << endl;
                }
                else {
                    std::cout << "Apache Attack Helicopter, or something" << endl;
                }
            }
        }

        if (game.Input.IsButtonPressedThisFrame(ButtonCode::Middle) || game.Input.WasQuitCalled()) {
            game.Stop();
        }

        auto var = game.Input.GetMousePosition();
        auto vect = Game::Vector2(var.first - 960.0, var.second - 540.0);
        auto angle = vect.Angle();
        entity->GetComponent().SetDirection(angle);

	}

    void PlayerPseudoAI::OnDeath() {

    } 
    
    void PlayerPseudoAI::OnAttackHit() {

    }

    void PlayerPseudoAI::OnHitByAttack(Actor* attacker, double damage) {

    }
}