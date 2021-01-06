#include "PCHeader.h"
#include "PlayerPseudoAI.h"
#include "Globals.h"
#include "Actor.h"

// This is an AI in name only. In reality, this class interfaces with inputs
// From the game's engine

namespace Game {
    PlayerPseudoAI::PlayerPseudoAI():
        shieldFadeOutDelay(0),
        shieldRegenCounter(0),
        perfectGuardCounter(0),
        currentPistolAmmo(15),
        maxPistolAmmo(15),
        regenCounter(0),
        wasReloadingPistol(false),
        hasShield(true),
        inShield(false),
        AI()
    {
        playerHealth.SetFont("Huge");
        playerHealth.SetRelativeToCamera(false);
        playerHealth.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
        playerHealth.SetPosition(Vector2(10, 20));
        playerHealth.RegisterToGame();

        shieldHealth.SetFont("Big");
        shieldHealth.SetRelativeToCamera(false);
        shieldHealth.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
        shieldHealth.SetPosition(Vector2(20, 155));
        shieldHealth.RegisterToGame();

        gunAmmo.SetFont("Big");
        gunAmmo.SetRelativeToCamera(false);
        gunAmmo.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
        gunAmmo.SetPosition(Vector2(20, 95));
        gunAmmo.RegisterToGame();
    }

    PlayerPseudoAI::~PlayerPseudoAI() {
        playerHealth.UnregisterFromGame();
        gunAmmo.UnregisterFromGame();
        shieldHealth.UnregisterFromGame();
    }

	void PlayerPseudoAI::Update() {
		if (entity == nullptr) {
			return;
		}

        if(shieldFadeOutDelay > 0) {
            shieldFadeOutDelay--;
        }

        if (perfectGuardCounter > 0) {
            perfectGuardCounter--;
        }

        if (entity->GetStatsReference().isDead) {
            playerHealth.SetText("Dead!");
        }
        else {
            regenCounter++;
            if (regenCounter >= 60 + (Globals::Difficulty() > 1 ? 30 : 0)) {
                regenCounter = 0;
                if (entity->GetStatsReference().health < entity->GetStatsReference().maxHealth) {
                    double healing = 3.0 - Globals::Difficulty();

                    entity->GetStatsReference().health += healing;
                    if (entity->GetStatsReference().health > entity->GetStatsReference().maxHealth) {
                        entity->GetStatsReference().health = entity->GetStatsReference().maxHealth;
                    }
                }
            }

            shieldRegenCounter++;

            if (shieldRegenCounter >= 150) {
                shieldRegenCounter = 120;
                if (entity->GetStatsReference().shieldHealth < entity->GetStatsReference().maxShieldHealth) {
                    double healing = 8.0 - Globals::Difficulty() * 2;

                    entity->GetStatsReference().shieldHealth += healing;
                    if (entity->GetStatsReference().shieldHealth > entity->GetStatsReference().maxShieldHealth) {
                        entity->GetStatsReference().shieldHealth = entity->GetStatsReference().maxShieldHealth;
                    }
                    shieldFadeOutDelay = 40;
                }
            }

            if (inShield || entity->GetStatsReference().shieldHealth < entity->GetStatsReference().maxShieldHealth && shieldRegenCounter >= 0) {
                shieldHealth.SetAlpha(255);
                shieldHealth.SetText("Shield: " + std::to_string(int(entity->GetStatsReference().shieldHealth)) + " / " + std::to_string(int(entity->GetStatsReference().maxShieldHealth)));
            }
            else if (shieldRegenCounter < 0) {
                shieldHealth.SetAlpha(255);
                shieldHealth.SetText("Shield broken!");
            }
            else {
                if (shieldFadeOutDelay <= 30) {
                    shieldHealth.SetAlpha(shieldFadeOutDelay * 255 / 30);
                }
            }

            playerHealth.SetText("Health: " + std::to_string(int(entity->GetStatsReference().health)) + " / " + std::to_string(int(entity->GetStatsReference().maxHealth)));
        }

        gunAmmo.SetText("Ammo: " + std::to_string(currentPistolAmmo) + " / " + std::to_string(maxPistolAmmo));

        if (wasReloadingPistol == true && entity->GetComponent().GetCurrentAnimationID() == "PlayerIdle") {
            currentPistolAmmo = maxPistolAmmo;
            wasReloadingPistol = false;
        }

        auto& game = Globals::Game();
        using KeyCode = Game::InputHandler::KeyCode;
        using ButtonCode = Game::InputHandler::ButtonCode;

        if (game.Input.IsButtonPressedThisFrame(ButtonCode::Middle) || game.Input.WasQuitCalled()) {
            //game.Stop();
        }

        if (entity->GetStatsReference().isDead) {
            return;
        }

        double moveSpeed = 9.0;
        if (entity->GetComponent().GetCurrentAnimationID() == "PlayerReload") {
            moveSpeed /= 3.0;
        }
        else if (inShield) {
            moveSpeed /= 2.5;
        }


        if (game.Input.IsKeyDown(KeyCode::W)) {
            entity->Move({ 0.0, -moveSpeed });
        }
        if (game.Input.IsKeyDown(KeyCode::S)) {
            entity->Move({ 0.0, moveSpeed });
        }
        if (game.Input.IsKeyDown(KeyCode::A)) {
            entity->Move({ -moveSpeed, 0.0 });
        }
        if (game.Input.IsKeyDown(KeyCode::D)) {
            entity->Move({ moveSpeed, 0.0 });
        }


        auto currentAnimation = entity->GetComponent().GetCurrentAnimationID();
        if (shieldRegenCounter >= 0 && game.Input.IsButtonPressedThisFrame(ButtonCode::Right) && currentAnimation != "CharShieldUp" && currentAnimation != "CharShieldWalk" && currentAnimation != "CharShieldDown") {
            wasReloadingPistol = false;
            inShield = true;
            perfectGuardCounter = 16;
            entity->GetComponent().SwitchAnimation("CharShieldUp");
        }

        if ((!game.Input.IsButtonDown(ButtonCode::Right) || shieldRegenCounter < 0) && currentAnimation == "CharShieldWalk") {
            entity->GetComponent().SwitchAnimation("CharShieldDown");
            perfectGuardCounter = 0;
            inShield = false;
            shieldFadeOutDelay = 30;
        }

        if (game.Input.IsButtonPressedThisFrame(ButtonCode::Left) && entity->GetComponent().GetCurrentAnimationID() == "PlayerIdle") {
            if (currentPistolAmmo > 0) {
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
                currentPistolAmmo--;
            }
            else {
                Globals::Audio().PlaySound("GunClick");
            }
        }

        if (game.Input.IsKeyPressedThisFrame(KeyCode::R) && entity->GetComponent().GetCurrentAnimationID() == "PlayerIdle") {
            entity->GetComponent().SwitchAnimation("PlayerReload");
            wasReloadingPistol = true;
        }

        auto var = game.Input.GetMousePosition();
        auto vect = Game::Vector2(var.first - 960.0, var.second - 540.0);
        auto angle = vect.Angle();
        entity->GetComponent().SetDirection(angle);

	}

    void PlayerPseudoAI::OnDeath() {
        if (entity != nullptr) {
            AI::OnDeath();
            entity->GetComponent().SwitchAnimation("CharDead");
            entity->GetCollider().QueueUnregisterFromGame();
            entity->GetComponent().SetLayer(GraphicsEngine::CommonLayers::OnFloor);
        }
    } 
    
    void PlayerPseudoAI::OnAttackHit() {
        
    }

    void PlayerPseudoAI::OnHitByAttack(Actor* attacker, double damage) {
        regenCounter = -120;
        if (entity != nullptr) {
            auto& stats = entity->GetStatsReference();
            if (damage < 0.0) {
                return;
            }
            if (stats.currentInvincibilityFrames > 0) {
                return;
            }
            stats.currentInvincibilityFrames = stats.onHitInvincibilityFrames;


            bool shieldBlock = false;
            if (inShield) {
                auto attackAngle = (attacker->GetTransform().position - entity->GetTransform().position).Angle();
                auto angleDelta = abs(attackAngle - entity->GetTransform().direction);
                angleDelta = angleDelta > 180.0 ? 360.0 - angleDelta : angleDelta;
                if (angleDelta < 60.0) {
                    shieldBlock = true;
                    Globals::Audio().PlaySound("ShieldImpact");
                }
            }
            if (shieldBlock) {
                if (perfectGuardCounter > 0) {
                    damage *= 0.5;
                }
                stats.shieldHealth -= damage;
                shieldRegenCounter = 0;
                if (stats.shieldHealth <= 0.0) {
                    stats.shieldHealth = 0.0;
                    shieldRegenCounter = -240;
                    stats.currentInvincibilityFrames = 40;
                }
            }
            else {
                stats.health -= damage;
            }
            if (stats.health <= 0.0) {
                OnDeath();
                std::cout << "Your guy is DEAD!" << endl;
            }
        }
    }
}