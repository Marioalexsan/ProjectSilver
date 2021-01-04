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

        if (game.Input.IsButtonPressedThisFrame(ButtonCode::Left) && entity->GetComponent().GetCurrentAnimationID() != "PlayerShoot") {
            auto mouse = game.Input.GetMousePosition();
            auto shotAngle = (Game::Vector2(mouse.first, mouse.second) - Game::Vector2(960.0, 540.0)).Angle();
            auto results = game.CreateRayCastHitList(entity->GetCollider().GetPosition(), Vector2::NormalVector(shotAngle) * 1800.0 + entity->GetCollider().GetPosition());
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
            if (results.size() > 1) {
                // First collider is usually the player's own, which is crappy, but c'est la vie
                auto firstElem = results[0].second->GetEntity() == Globals::Game().GetThePlayer() ? results[1] : results[0];
                auto entity = firstElem.second->GetEntity();
                if (dynamic_cast<Actor*>(entity) != nullptr) {
                    auto actor = (Actor*)entity;
                    auto actorAI = actor->GetAI();
                    if (actorAI != nullptr) {
                        actorAI->OnHitByAttack(this->entity, 20.0);
                    }
                }
            }
            entity->GetComponent().SwitchAnimation("PlayerShoot");
        }

        if (game.Input.IsButtonPressedThisFrame(ButtonCode::Right)) {
            game.AddNewEnemy(ActorType::Fighter, Game::Vector2(-100.0, -100.0));
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