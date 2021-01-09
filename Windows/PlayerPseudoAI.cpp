#include "PCHeader.h"
#include "PlayerPseudoAI.h"
#include "Globals.h"
#include "Actor.h"

// This is an AI in name only. In reality, this class interfaces with inputs
// From the game's engine

namespace Game {
    PlayerPseudoAI::PlayerPseudoAI():
        wasInWeaponSwitch(false),
        currentRifleAmmo(15),
        maxRifleAmmo(30),
        rifleAmmoPool(0),
        equippedWeapon(0),
        targetWeaponEquip(-1),
        boostCooldown(0),
        staminaFadeOutDelay(0),
        heartbeatCounter(0),
        heartbeatTime(0),
        staminaRegenCounter(0),
        vignetteCounter(30),
        doingSwing(false),
        smoothSpeed(0.0, 0.0),
        shieldFadeOutDelay(0),
        shieldRegenCounter(0),
        perfectGuardCounter(0),
        currentPistolAmmo(12),
        maxPistolAmmo(12),
        regenCounter(0),
        wasReloadingPistol(false),
        wasReloadingRifle(false),
        hasShield(true),
        inShield(false),
        axe(Game::Vector2(0, 0), 60, Game::Collider::ColliderType::Combat),
        AI()
    {

        axe.SetCombatDamage(100.0);
        axe.SetCombatLayer(Collider::CombatLayer::None);
        axe.SetLayersToAttack({ Collider::CombatLayer::Enemies });
        axe.SetCollisionOptions({ Collider::CollisionOptions::DoNotHitRememberedEnemies });

        playerHealth.SetFont("Huge");
        playerHealth.SetRelativeToCamera(false);
        playerHealth.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
        playerHealth.SetPosition(Vector2(10, 20));
        playerHealth.RegisterToGame();

        shieldHealth.SetFont("Big");
        shieldHealth.SetRelativeToCamera(false);
        shieldHealth.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
        shieldHealth.SetPosition(Vector2(20, 95));
        shieldHealth.RegisterToGame();

        stamina.SetFont("Big");
        stamina.SetRelativeToCamera(false);
        stamina.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
        stamina.SetPosition(Vector2(20, 155));
        stamina.RegisterToGame();

        lowHPVignette.SetTexture("LowHP");
        lowHPVignette.SetRelativeToCamera(false);
        lowHPVignette.SetLayer(Game::GraphicsEngine::CommonLayers::AboveGUI);
        lowHPVignette.SetAlpha(0);
        lowHPVignette.RegisterToGame();

        gunAmmo.SetFont("Big");
        gunAmmo.SetRelativeToCamera(false);
        gunAmmo.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
        gunAmmo.SetPosition(Vector2(20, 1020));
        gunAmmo.RegisterToGame();
    }

    PlayerPseudoAI::~PlayerPseudoAI() {
        playerHealth.UnregisterFromGame();
        gunAmmo.UnregisterFromGame();
        shieldHealth.UnregisterFromGame();
        lowHPVignette.UnregisterFromGame();
        stamina.UnregisterFromGame();
    }

	void PlayerPseudoAI::Update() {
        static const vector<string> uninterruptibleAnimations = {
            "CharShieldUp",
            "CharShieldWalk",
            "CharShieldDown",
            "CharAxeSwing"
        };

        static const vector<string> switchAnimations = {
            "TakeOutRifle",
            "TakeOutPistol",
            "PutAwayRifle",
            "PutAwayPistol"
        };

        auto IsUninterruptible = [](const string & animation) {
            return std::find(uninterruptibleAnimations.begin(), uninterruptibleAnimations.end(), animation) != uninterruptibleAnimations.end();
        };

        auto IsWeaponSwitch = [](const string& animation) {
            return std::find(switchAnimations.begin(), switchAnimations.end(), animation) != switchAnimations.end();
        };


		if (entity == nullptr) {
			return;
		}

        auto& stats = entity->GetStatsReference();

        auto vignetteAlpha = lowHPVignette.GetAlpha();
        if (stats.health <= 0.0 || stats.maxHealth <= 20.0) {
            if (vignetteCounter % 5 == 0) {
                lowHPVignette.SetAlpha(vignetteAlpha + (255 - vignetteAlpha) * 0.04);
            }
            heartbeatTime = 120;
        }
        else {
            if (vignetteCounter % 8 == 0) {
                lowHPVignette.SetAlpha(vignetteAlpha + (0 - vignetteAlpha) * 0.04);
            }
        }

        if (heartbeatTime > 0) {
            heartbeatTime--;
        }

        if (heartbeatCounter > 0) {
            heartbeatCounter--;
        }

        if(shieldFadeOutDelay > 0) {
            shieldFadeOutDelay--;
        }

        if (staminaFadeOutDelay > 0) {
            staminaFadeOutDelay--;
        }


        if (boostCooldown > 0) {
            boostCooldown--;
        }

        if (perfectGuardCounter > 0) {
            perfectGuardCounter--;
        }

        if (vignetteCounter > 0) {
            vignetteCounter--;
            if (vignetteCounter == 0) {
                vignetteCounter = 30;
            }
        }

        if (stats.isDead) {
            playerHealth.SetText("Dead!");
        }
        else {
            regenCounter++;
            if (regenCounter >= 60) {
                regenCounter = 0;

                double healing = 3.0 - Globals::Difficulty();

                if (stats.maxHealth <= 100.0 && stats.health == stats.maxHealth) {
                    stats.maxHealth += healing;
                    regenCounter -= 75 + 15 * Globals::Difficulty();
                    if (stats.maxHealth > 100.0) {
                        stats.maxHealth = 100.0;
                    }
                }

                if (stats.health < stats.maxHealth) {
                    stats.health += healing;
                    if (stats.health > stats.maxHealth) {
                        stats.health = stats.maxHealth;
                    }
                }
            }

            shieldRegenCounter++;

            if (shieldRegenCounter >= 180) {
                shieldRegenCounter = 150;
                if (stats.shieldHealth < stats.maxShieldHealth) {
                    double healing = 6.0 - Globals::Difficulty();

                    stats.shieldHealth += healing;
                    if (stats.shieldHealth > stats.maxShieldHealth) {
                        stats.shieldHealth = stats.maxShieldHealth;
                    }
                    shieldFadeOutDelay = 40;
                }
            }

            staminaRegenCounter++;

            if (staminaRegenCounter >= 150) {
                staminaRegenCounter -= 5;
                if (stats.stamina < stats.maxStamina) {
                    stats.stamina += 1.0;
                    if (stats.stamina > stats.maxStamina) {
                        stats.stamina = stats.maxStamina;
                    }
                    staminaFadeOutDelay = 40;
                }
            }

            if (inShield || stats.shieldHealth < stats.maxShieldHealth && shieldRegenCounter >= 0) {
                shieldHealth.SetAlpha(255);
                shieldHealth.SetText("Shield: " + std::to_string(int(stats.shieldHealth)) + " / " + std::to_string(int(stats.maxShieldHealth)));
            }
            else if (shieldRegenCounter < 0) {
                shieldHealth.SetAlpha(255);
                shieldHealth.SetText("Shield broken!");
            }
            else {
                if (shieldFadeOutDelay <= 30) {
                    shieldHealth.SetText("Shield: " + std::to_string(int(stats.shieldHealth)) + " / " + std::to_string(int(stats.maxShieldHealth)));
                    shieldHealth.SetAlpha(shieldFadeOutDelay * 255 / 30);
                }
            }

            playerHealth.SetText("Health: " + std::to_string(int(stats.health)) + " / " + std::to_string(int(stats.maxHealth)));
            if (stats.health <= 0.0 || stats.maxHealth <= 20.0) {
                playerHealth.SetColor(Color::Orange);
            }
            else {
                playerHealth.SetColor(Color::White);
            }

            stamina.SetText("Boost: " + std::to_string(int(stats.stamina / 45.0)) + " / " + std::to_string(int(stats.maxStamina / 45.0)));
            if (stats.stamina < 45.0) {
                stamina.SetColor(Color::Orange);
            }
            else{
                stamina.SetColor(Color::White);
            }
            if (stats.stamina < stats.maxStamina) {
                stamina.SetAlpha(255);
            }
            else {
                if (staminaFadeOutDelay <= 30) {
                    stamina.SetAlpha(staminaFadeOutDelay * 255 / 30);
                }
            }
        }


        auto vol = Globals::Audio().GetMusicVolume();
        if (heartbeatTime > 0) {
            if (vignetteCounter % 2 == 0) {
                Globals::Audio().SetMusicVolume(Utility::ClampValue(vol - 1.0, 35.0, 100.0));
            }
            
            if (heartbeatCounter == 0) {
                heartbeatCounter = 48;
                Globals::Audio().PlaySound("Heartbeat");
            }
        }
        else {
            if (vignetteCounter % 2 == 0) {
                Globals::Audio().SetMusicVolume(Utility::ClampValue(vol + 1.0, 35.0, 100.0));
            }
        }
        if (equippedWeapon == 0) {
            gunAmmo.SetText("Ammo: " + std::to_string(currentPistolAmmo) + " / " + std::to_string(maxPistolAmmo));
            if (currentPistolAmmo == 0) {
                gunAmmo.SetColor(Color::Orange);
            }
            else {
                gunAmmo.SetColor(Color::White);
            }
        }
        else {
            gunAmmo.SetText("Ammo: " + std::to_string(currentRifleAmmo) + " / " + std::to_string(maxRifleAmmo) + " (" + std::to_string(rifleAmmoPool) + ")" + (currentRifleAmmo + rifleAmmoPool >= 60 ? " (Max!)" : ""));
            if (currentRifleAmmo == 0) {
                if (rifleAmmoPool == 0) {
                    gunAmmo.SetColor(Color::Orange);
                }
                else {
                    gunAmmo.SetColor(Color::Yellow);
                }
            }
            else {
                gunAmmo.SetColor(Color::White);
            }
        }
        


        if (wasReloadingPistol == true && entity->GetComponent().GetCurrentAnimationID() == "PlayerIdle") {
            currentPistolAmmo = maxPistolAmmo;
            wasReloadingPistol = false;
        }

        if (wasReloadingRifle == true && entity->GetComponent().GetCurrentAnimationID() == "CharRifleIdle") {
            int reloadAmount = maxRifleAmmo - currentRifleAmmo;
            if (reloadAmount > rifleAmmoPool) {
                reloadAmount = rifleAmmoPool;
            }
            currentRifleAmmo += reloadAmount;
            rifleAmmoPool -= reloadAmount;
            wasReloadingRifle = false;
        }

        auto& game = Globals::Game();
        using KeyCode = Game::InputHandler::KeyCode;
        using ButtonCode = Game::InputHandler::ButtonCode;

        if (game.Input.IsButtonPressedThisFrame(ButtonCode::Middle) || game.Input.WasQuitCalled()) {
            //game.Stop();
        }

        if (stats.isDead) {
            return;
        }

        double moveSpeed = 9.0;
        if (entity->GetComponent().GetCurrentAnimationID() == "PlayerReload" || entity->GetComponent().GetCurrentAnimationID() == "RifleReload") {
            moveSpeed /= 3.0;
        }
        else if (entity->GetComponent().GetCurrentAnimationID() == "RifleShoot") {
            moveSpeed /= 1.5;
        }
        else if (inShield) {
            moveSpeed /= 1.25;
        }
        else if (entity->GetComponent().GetCurrentAnimationID() == "CharAxeSwing") {
            int currentFrame = entity->GetComponent().GetFrame();
            double factor = 0.0;
            if (currentFrame < 3) {
                factor = 0.2;
            }
            else if (4 <= currentFrame && currentFrame <= 6) {
                factor = 2.3 * Utility::ClampValue(currentFrame - 3, 0, 3) / 3;
            }
            else if (currentFrame <= 8) {
                factor = 2.3;
            }
            else {
                factor = 2.0 * Utility::ClampValue(11 - currentFrame, 0, 3) / 3;
            }
            moveSpeed *= factor * 0.75;
        }

        int horizontalDir = 0;
        int verticalDir = 0;
        if (game.Input.IsKeyDown(KeyCode::W)) {
            verticalDir -= 1;
        }
        if (game.Input.IsKeyDown(KeyCode::S)) {
            verticalDir += 1;
        }
        if (game.Input.IsKeyDown(KeyCode::A)) {
            horizontalDir -= 1;
        }
        if (game.Input.IsKeyDown(KeyCode::D)) {
            horizontalDir += 1;
        }
        
        Vector2 targetSpeed = Vector2(0.0, 0.0);
        Vector2 targetSpeedNormal = Vector2::NormalVector(Vector2(horizontalDir, verticalDir).Angle());
        if (horizontalDir != 0 || verticalDir != 0) {
            targetSpeed = targetSpeedNormal * moveSpeed;
        }

        if (boostCooldown == 0 && stats.stamina >= 45.0 && targetSpeed != Vector2::Zero && Globals::Game().Input.IsKeyPressedThisFrame(KeyCode::LShift)) {
            stats.stamina -= 45.0;
            staminaRegenCounter = 0;
            boostVector = targetSpeedNormal * 130.0;
            boostCooldown = 18;
            Globals::Audio().PlaySound("Boost");
        }

        smoothSpeed = smoothSpeed + (targetSpeed - smoothSpeed) * 0.18;

        entity->Move(smoothSpeed);
        if (boostVector != Vector2::Zero) {
            entity->Move(boostVector * 0.2);
            boostVector *= 0.8;
            if (boostVector.Length() < 1.0) {
                boostVector = Vector2::Zero;
            }
        }
        
        if (shieldRegenCounter >= 0 && game.Input.IsButtonDown(ButtonCode::Right) && !IsUninterruptible(entity->GetComponent().GetCurrentAnimationID()) && !IsWeaponSwitch(entity->GetComponent().GetCurrentAnimationID())) {
            wasReloadingPistol = false;
            inShield = true;
            perfectGuardCounter = 12;
            entity->GetComponent().SwitchAnimation("CharShieldUp");
        }

        if ((!game.Input.IsButtonDown(ButtonCode::Right) || shieldRegenCounter < 0) && entity->GetComponent().GetCurrentAnimationID() == "CharShieldWalk") {
            entity->GetComponent().SwitchAnimation("CharShieldDown");
            perfectGuardCounter = 0;
            shieldFadeOutDelay = 30;
        }

        if (game.Input.IsButtonDown(ButtonCode::Left)) {
            if (game.Input.IsButtonPressedThisFrame(ButtonCode::Left) && equippedWeapon == 0 && entity->GetComponent().GetCurrentAnimationID() == "PlayerIdle") {
                if (currentPistolAmmo > 0) {
                    auto mouse = game.Input.GetMousePosition();
                    auto shotAngle = (Game::Vector2(mouse.first, mouse.second) - Game::Vector2(960.0, 540.0)).Angle();
                    auto results = game.CreateRayCastHitList(entity->GetCollider().GetPosition(), Vector2::NormalVector(shotAngle) * 1800.0 + entity->GetCollider().GetPosition());
                    if (results.size() > 1) {
                        // First collider is usually the player's own, which is crappy, but c'est la vie
                        auto firstElem = results[0].second->GetEntity() == Globals::Game().GetThePlayer() ? results[1] : results[0];
                        auto entity = firstElem.second->GetEntity();
                        if (dynamic_cast<Actor*>(entity) != nullptr) {
                            auto actor = (Actor*)entity;
                            auto actorAI = actor->GetAI();
                            if (actorAI != nullptr) {
                                actorAI->OnHitByAttack(this->entity, 35.0);
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
            else if(equippedWeapon == 1 && (entity->GetComponent().GetCurrentAnimationID() == "CharRifleIdle" || entity->GetComponent().GetCurrentAnimationID() == "RifleShoot" && entity->GetComponent().GetFrame() == 4)) {
                if (currentRifleAmmo > 0) {
                    auto mouse = game.Input.GetMousePosition();
                    auto shotAngle = (Game::Vector2(mouse.first, mouse.second) - Game::Vector2(960.0, 540.0)).Angle();
                    auto results = game.CreateRayCastHitList(entity->GetCollider().GetPosition(), Vector2::NormalVector(shotAngle) * 1800.0 + entity->GetCollider().GetPosition());
                    if (results.size() > 1) {
                        // First collider is usually the player's own, which is crappy, but c'est la vie
                        auto firstElem = results[0].second->GetEntity() == Globals::Game().GetThePlayer() ? results[1] : results[0];
                        auto entity = firstElem.second->GetEntity();
                        if (dynamic_cast<Actor*>(entity) != nullptr) {
                            auto actor = (Actor*)entity;
                            auto actorAI = actor->GetAI();
                            if (actorAI != nullptr) {
                                actorAI->OnHitByAttack(this->entity, 38.0);
                            }
                        }
                    }
                    entity->GetComponent().SwitchAnimation("RifleShoot");
                    currentRifleAmmo--;
                }
                else {
                    if (game.Input.IsButtonPressedThisFrame(ButtonCode::Left)) {
                        Globals::Audio().PlaySound("GunClick");
                    }
                }
            }
        }

        if (game.Input.IsKeyDown(KeyCode::E) && (entity->GetComponent().GetCurrentAnimationID() != "CharAxeSwing") && !IsWeaponSwitch(entity->GetComponent().GetCurrentAnimationID())) {
            wasReloadingPistol = false;
            wasReloadingRifle = false;
            entity->GetComponent().SwitchAnimation("CharAxeSwing");
            axe.GetHitList().clear();
        }

        string currentID = entity->GetComponent().GetCurrentAnimationID();
        if (currentID != "CharShieldWalk" && currentID != "CharShieldUp" && currentID != "CharShieldDown") {
            inShield = false;
        }

        if (entity->GetComponent().GetCurrentAnimationID() == "CharAxeSwing") {
            int currentFrame = entity->GetComponent().GetFrame();

            axe.SetPosition(entity->GetTransform().position + Vector2::NormalVector(entity->GetTransform().direction) * 20);

            if (currentFrame == 5 && !doingSwing) {
                doingSwing = true;
                axe.SetOwner(entity);
                axe.RegisterToGame();
            }

            if (currentFrame == 8) {
                doingSwing = false;
                axe.UnregisterFromGame();
            }
        }
        else {
            axe.UnregisterFromGame();
        }

        if (wasInWeaponSwitch) {
            auto animationID = entity->GetComponent().GetCurrentAnimationID();
            auto& component = entity->GetComponent();
            bool switchCondition = component.GetFrame() == component.GetFrameCount() - 1 && component.GetAccumulatedUpdates() >= component.GetUpdatesPerFrame() - 2;
            if (targetWeaponEquip == 0 && (animationID == "PlayerIdle" || animationID == "PutAwayRifle" && switchCondition)) {
                entity->GetComponent().SwitchAnimation("TakeOutPistol");
                equippedWeapon = 0;
                wasInWeaponSwitch = false;
            }
            if (targetWeaponEquip == 1 && (animationID == "CharRifleIdle" || animationID == "PutAwayPistol" && switchCondition)) {
                entity->GetComponent().SwitchAnimation("TakeOutRifle");
                equippedWeapon = 1;
                wasInWeaponSwitch = false;
            }
        }

        if (game.Input.IsKeyPressedThisFrame(KeyCode::NumOne) && equippedWeapon != 0) {
            if (entity->GetComponent().GetCurrentAnimationID() == "CharRifleIdle") {
                entity->GetComponent().SwitchAnimation("PutAwayRifle");
                entity->GetComponent().Update();
                entity->GetComponent().SetDefaultAnimation("PlayerIdle");
                targetWeaponEquip = 0;
                wasInWeaponSwitch = true;
            }
        }

        if (game.Input.IsKeyPressedThisFrame(KeyCode::NumTwo) && equippedWeapon != 1) {
            if (entity->GetComponent().GetCurrentAnimationID() == "PlayerIdle") {
                entity->GetComponent().SwitchAnimation("PutAwayPistol");
                entity->GetComponent().Update();
                entity->GetComponent().SetDefaultAnimation("CharRifleIdle");
                targetWeaponEquip = 1;
                wasInWeaponSwitch = true;
            }
        }

        if (game.Input.IsKeyPressedThisFrame(KeyCode::R)) {
            if (entity->GetComponent().GetCurrentAnimationID() == "PlayerIdle" && currentPistolAmmo < maxPistolAmmo) {
                entity->GetComponent().SwitchAnimation("PlayerReload");
                wasReloadingPistol = true;
            }
            else if (entity->GetComponent().GetCurrentAnimationID() == "CharRifleIdle" && currentRifleAmmo < maxRifleAmmo && rifleAmmoPool > 0) {
                entity->GetComponent().SwitchAnimation("RifleReload");
                wasReloadingRifle = true;
            }
        }

        auto var = game.Input.GetMousePosition();
        auto vect = Game::Vector2(var.first - 960.0, var.second - 540.0);
        auto angle = vect.Angle();
        entity->GetComponent().SetDirection(angle);

	}

    void PlayerPseudoAI::OnDeath() {
        if (entity != nullptr) {
            AI::OnDeath();
            Globals::Audio().PlaySound("PlayerDeath");
            axe.QueueUnregisterFromGame();
            entity->GetComponent().SwitchAnimation("CharDead");
            entity->GetCollider().QueueUnregisterFromGame();
            entity->GetComponent().SetLayer(GraphicsEngine::CommonLayers::OnFloor);
        }
    } 
    
    void PlayerPseudoAI::OnAttackHit() {
        
    }

    void PlayerPseudoAI::OnHitByAttack(Actor* attacker, double damage) {
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
                if (angleDelta < 90.0) {
                    shieldBlock = true;
                    Globals::Audio().PlaySound("ShieldImpact");
                }
            }
            if (shieldBlock) {
                if (perfectGuardCounter > 0) {
                    damage *= 0.35;
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
                regenCounter = -120 - Globals::Difficulty() * 20;
                if (stats.health > 0.0) {
                    Globals::Audio().PlaySound("PlayerHurt");
                }
                else {
                    stats.maxHealth += stats.health;
                    stats.health = 0.0;
                    if (stats.maxHealth <= 0.0) {
                        OnDeath();
                        std::cout << "Your guy is DEAD!" << endl;
                    }
                    else {
                        Globals::Audio().PlaySound("PlayerHurt");
                    }
                }
            }
        }
    }
}