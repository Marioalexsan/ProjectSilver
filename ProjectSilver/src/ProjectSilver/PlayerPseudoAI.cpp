#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/PlayerPseudoAI.hpp>
#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/Actor.hpp>

// This is an AI in name only. This class interfaces with inputs, providing behavior for the Player's character

namespace Game {
    PlayerPseudoAI::PlayerPseudoAI():
        shootPenaltyCountdown(0),
        currentPistolAmmo(12),
        maxPistolAmmo(12),
        wasReloadingPistol(false),

        currentRifleAmmo(30),
        maxRifleAmmo(30),
        rifleAmmoPool(0),
        rifleRecoil(0.0),
        wasReloadingRifle(false),

        hasShield(true),
        inShield(false),
        shieldFadeOutDelay(0),
        shieldRegenCounter(0),
        perfectGuardCounter(0),
        hasPerfectGuarded(false),

        doingSwing(false),

        wasInWeaponSwitch(false),

        regenCounter(0),
        
        staminaRegenCounter(0),
        staminaFadeOutDelay(0),
        boostCooldown(0),

        heartbeatTime(0),
        heartbeatCounter(0),
        vignetteCounter(30),

        equippedWeapon(0),
        targetWeaponEquip(-1),
        boostVector(0.0, 0.0),
        
        axe(Game::Vector2(0, 0), 60, Game::Collider::ColliderType::Combat),
        AI()
    {
        axe.SetCombatDamage(100.0);
        axe.SetCombatLayer(Collider::CombatLayer::None);
        axe.SetLayersToAttack({ Collider::CombatLayer::Enemies });
        axe.SetCollisionOptions({ Collider::CollisionOptions::DoNotHitRememberedEntities });

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
        // Unregisters should be handled by the destructors
    }

	void PlayerPseudoAI::Update() {
        static const std::vector<std::string> uninterruptibleAnimations = {
            "Player_ShieldUp",
            "Player_ShieldWalk",
            "Player_ShieldDown",
            "Player_AxeSwing"
        };

        static const std::vector<std::string> switchAnimations = {
            "Player_TakeOutRifle",
            "Player_TakeOutPistol",
            "Player_PutAwayRifle",
            "Player_PutAwayPistol"
        };

        if (entity == nullptr) {
			return;
		}

        auto IsUninterruptible = [&](const std::string & animation) {
            return std::find(uninterruptibleAnimations.begin(), uninterruptibleAnimations.end(), animation) != uninterruptibleAnimations.end();
        };

        auto IsWeaponSwitch = [&](const std::string& animation) {
            return std::find(switchAnimations.begin(), switchAnimations.end(), animation) != switchAnimations.end();
        };

        auto& stats = entity->GetStatsReference();
        auto difficultySetting = Globals::Difficulty();
        auto& playerAnimation = entity->GetComponent().GetCurrentAnimationID();
        auto& game = Globals::Game();
        using KeyCode = Game::InputHandler::KeyCode;
        using ButtonCode = Game::InputHandler::ButtonCode;

        counter++;

        // Low HP Vignette. Serves as a "You're taking too much damage" warning for the player
        uint8_t vignetteAlpha = lowHPVignette.GetAlpha();
        if (stats.health <= 0.0 || stats.maxHealth <= 20.0) {
            if (vignetteCounter % 3 == 0) {
                lowHPVignette.SetAlpha(vignetteAlpha + uint8_t(ceil((255 - vignetteAlpha) * 0.04)));
            }
            heartbeatTime = 60;
            if (stats.maxHealth <= 60.0) {
                heartbeatTime = 150;
            }
        }
        else {
            if (vignetteCounter % 4 == 0) {
                lowHPVignette.SetAlpha(vignetteAlpha - uint8_t(ceil(vignetteAlpha * 0.04)));
            }
        }


        // Regular Counters (down)
        if (heartbeatTime > 0) { heartbeatTime--; }
        if (heartbeatCounter > 0) { heartbeatCounter--; }
        if (shieldFadeOutDelay > 0) { shieldFadeOutDelay--; }
        if (staminaFadeOutDelay > 0) { staminaFadeOutDelay--; }
        if (boostCooldown > 0) { boostCooldown--; }
        if (perfectGuardCounter > 0) { perfectGuardCounter--; }

        if (rifleRecoil > 0.0) {
            rifleRecoil *= 0.98;
            if (rifleRecoil >= 5.0) {
                rifleRecoil *= 0.96;
            }
            rifleRecoil -= 0.1;
            if (rifleRecoil <= 0.4) {
                rifleRecoil = 0.0;
            }
        }

        // Vignette Counter (periodic)
        // Could probably be replaced / removed
        if (vignetteCounter > 0) {
            vignetteCounter--;
            if (vignetteCounter == 0) {
                vignetteCounter = 30;
            }
        }


        // Regeneration code + GUI elements
        if (stats.isDead) {
            playerHealth.SetText("Dead!");
            playerHealth.SetColor(Color::Red);
        }
        else {

            regenCounter++;
            if (regenCounter >= 60) {
                regenCounter = 0;
                double healing = 3.0 - difficultySetting;
                
                if (stats.maxHealth <= 100.0 && stats.health == stats.maxHealth) {
                    stats.maxHealth = Utility::ClampValue(stats.maxHealth += healing, 0.0, 100.0);
                    regenCounter -= 75 + 15 * difficultySetting;
                }
                stats.health = Utility::ClampValue(stats.health += healing, 0.0, stats.maxHealth);
            }

            shieldRegenCounter++;
            if (shieldRegenCounter >= 120) {
                shieldRegenCounter = 90;
                double healing = 7.0 - difficultySetting;

                if (stats.shieldHealth < stats.maxShieldHealth) {
                    stats.shieldHealth = Utility::ClampValue(stats.shieldHealth += healing, 0.0, stats.maxShieldHealth);
                    shieldFadeOutDelay = 40;
                }
            }

            staminaRegenCounter++;
            if (staminaRegenCounter >= 120) {
                staminaRegenCounter -= 5;

                if (stats.stamina < stats.maxStamina) {
                    stats.stamina = Utility::ClampValue(stats.stamina += 1.0, 0.0, stats.maxStamina);
                    staminaFadeOutDelay = 40;
                }
            }

            if (inShield || stats.shieldHealth < stats.maxShieldHealth && shieldRegenCounter >= 0) {
                shieldHealth.SetAlpha(255);
                shieldHealth.SetText("Shield: " + std::to_string(int(stats.shieldHealth)) + " / " + std::to_string(int(stats.maxShieldHealth)));
                shieldHealth.SetColor(Color::White);
            }
            else if (shieldRegenCounter < 0) {
                shieldHealth.SetAlpha(255);
                shieldHealth.SetText("Shield broken!");
                shieldHealth.SetColor(Color::Orange);
            }
            else if (shieldFadeOutDelay <= 30) {
                    shieldHealth.SetText("Shield: " + std::to_string(int(stats.shieldHealth)) + " / " + std::to_string(int(stats.maxShieldHealth)));
                    shieldHealth.SetAlpha(shieldFadeOutDelay * 255 / 30);
                    shieldHealth.SetColor(Color::White);
            }

            playerHealth.SetText("Health: " + std::to_string(int(stats.health)) + " / " + std::to_string(int(stats.maxHealth)));
            if (stats.health <= 0.0 || stats.maxHealth <= 45.0) {
                if (stats.maxHealth <= 20.0) {
                    playerHealth.SetColor(Color::Red);
                }
                else {
                    playerHealth.SetColor(Color::Orange);
                }
            }
            else {
                playerHealth.SetColor(Color::White);
            }

            stamina.SetText("Boost: " + std::to_string(int(stats.stamina / 30.0)) + " / " + std::to_string(int(stats.maxStamina / 30.0)));
            if (stats.stamina <= 30.0) {
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

        // Low HP Audio effect
        double vol = Globals::Audio().GetMusicVolume();
        if (heartbeatTime > 0) {
            if (vignetteCounter % 2 == 0) {
                Globals::Audio().SetMusicVolume(Utility::ClampValue(vol - 1.0, 55.0, 100.0));
            }
            
            if (heartbeatCounter == 0) {
                heartbeatCounter = 48;
                if (stats.maxHealth <= 20.0) {
                    heartbeatCounter = 88;
                }
                Globals::Audio().PlaySound("Heartbeat");
            }
        }
        else {
            if (vignetteCounter % 2 == 0) {
                Globals::Audio().SetMusicVolume(Utility::ClampValue(vol + 1.0, 55.0, 100.0));
            }
        }

        // Ammo GUI element
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
        
        // Weapon Reload - Ammo Refill Logic
        if (wasReloadingPistol == true && entity->GetComponent().GetCurrentAnimationID() == "Player_PistolIdle") {
            currentPistolAmmo = maxPistolAmmo;
            wasReloadingPistol = false;
        }
        if (wasReloadingRifle == true && entity->GetComponent().GetCurrentAnimationID() == "Player_RifleIdle") {
            int reloadAmount = Utility::ClampValue(maxRifleAmmo - currentRifleAmmo, 0, rifleAmmoPool);
            currentRifleAmmo += reloadAmount;
            rifleAmmoPool -= reloadAmount;
            wasReloadingRifle = false;
        }

        if (stats.isDead) {
            return;
        }

        if (playerAnimation == "Player_PistolShoot" || playerAnimation == "Player_RifleShoot") {
            // No running and gunning, noob!
            shootPenaltyCountdown = 9;
        }

        // Move Speed Logic
        double moveSpeed = 9.0;
        if (playerAnimation == "Player_PistolReload" || playerAnimation == "Player_RifleReload") {
            moveSpeed /= 3.0;
        }
        else if (inShield) {
            moveSpeed /= 1.25;
        }
        else if (playerAnimation == "Player_AxeSwing") {
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
        else if (shootPenaltyCountdown > 0) {
            if (playerAnimation == "Player_RifleShoot") {
                // Positioning is key while using the rifle
                moveSpeed /= 1.5;
            }
            shootPenaltyCountdown--;
            moveSpeed /= 1.7;
        }

        // Move Speed Direction
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
        
        // This smooths out the player's movement
        Vector2 targetSpeed = Vector2(0.0, 0.0);
        Vector2 targetSpeedNormal = Vector2::NormalVector(Vector2(horizontalDir, verticalDir).Angle());
        if (horizontalDir != 0 || verticalDir != 0) {
            targetSpeed = targetSpeedNormal * moveSpeed;
        }

        // Dash mechanic logic
        if (boostCooldown == 0 && stats.stamina >= 30.0 && targetSpeed != Vector2::Zero && Globals::Game().Input.IsKeyPressedThisFrame(KeyCode::LShift)) {
            stats.stamina -= 30.0;
            staminaRegenCounter = 0;
            boostVector = targetSpeedNormal * 140.0;
            boostCooldown = 18;
            Globals::Audio().PlaySound("Boost");
        }

        // Movement is applied here
        smoothSpeed = smoothSpeed + (targetSpeed - smoothSpeed) * 0.18;
        entity->Move(smoothSpeed);

        // Boost is applied here - over time
        if (boostVector != Vector2::Zero) {
            entity->Move(boostVector * 0.2);
            if (counter % 3 == 0) {
                Globals::Game().CreateAfterImageEffect(entity->GetTransform(), entity->GetComponent().GetCurrentAnimationID(), entity->GetComponent().GetFrame());
            }
            boostVector *= 0.8;
            if (boostVector.Length() < 1.0) {
                boostVector = Vector2::Zero;
            }
        }
        
        // Shielding logic
        if (shieldRegenCounter >= 0 && game.Input.IsButtonDown(ButtonCode::Right) && (!IsUninterruptible(playerAnimation) || playerAnimation == "Player_ShieldDown" && hasPerfectGuarded ) && !IsWeaponSwitch(playerAnimation)) {
            wasReloadingPistol = false;
            wasReloadingRifle = false;
            inShield = true;
            perfectGuardCounter = 12;
            entity->GetComponent().SwitchAnimation("Player_ShieldUp");
        }
        if ((!game.Input.IsButtonDown(ButtonCode::Right) || shieldRegenCounter < 0) && playerAnimation == "Player_ShieldWalk") {
            entity->GetComponent().SwitchAnimation("Player_ShieldDown");
            hasPerfectGuarded = false;
            perfectGuardCounter = 0;
            shieldFadeOutDelay = 30;
        }

        // Shooting logic
        if (game.Input.IsButtonDown(ButtonCode::Left)) {
            if (equippedWeapon == 0 && playerAnimation == "Player_PistolIdle") {
                if (currentPistolAmmo > 0) {
                    // 1.5 degree spread
                    GenericWeaponFireLogic(35.0, -1.5 + rand() % 30 / 10.0);
                    entity->GetComponent().SwitchAnimation("Player_PistolShoot");
                    currentPistolAmmo--;
                }
                else
                {
                    entity->GetComponent().SwitchAnimation("Player_PistolReload");
                    wasReloadingPistol = true;
                }
            }
            else if(equippedWeapon == 1 && (playerAnimation == "Player_RifleIdle" || playerAnimation == "Player_RifleShoot" && entity->GetComponent().GetFrame() == 4)) {
                if (currentRifleAmmo > 0) {
                    // 0.5 + rifleRecoil degree spread
                    GenericWeaponFireLogic(50.0, -rifleRecoil / 2.0 + (rand() % (int(abs(rifleRecoil)) * 10 + 1)) / 10.0);
                    rifleRecoil += 4.8;
                    entity->GetComponent().SwitchAnimation("Player_RifleShoot");
                    currentRifleAmmo--;
                }
                else {
                    if (game.Input.IsButtonPressedThisFrame(ButtonCode::Left)) {
                        Globals::Audio().PlaySound("GunClick");
                    }
                }
            }
        }

        // Melee Attack Start Logic
        if (game.Input.IsKeyDown(KeyCode::E) && (playerAnimation != "Player_AxeSwing") && !IsWeaponSwitch(playerAnimation)) {
            wasReloadingPistol = false;
            wasReloadingRifle = false;
            entity->GetComponent().SwitchAnimation("Player_AxeSwing");
            axe.GetHitList().clear();
        }

        // Makes sure we're out of shield if the shield animation was cancelled (which can be done with Melee Attacks)
        if (playerAnimation != "Player_ShieldWalk" && playerAnimation != "Player_ShieldUp" && playerAnimation != "Player_ShieldDown") {
            inShield = false;
            hasPerfectGuarded = false;
        }

        // Melee Attack Execution Logic
        if (playerAnimation == "Player_AxeSwing") {
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

        // Weapon Switch Logic - Take Out Weapon
        if (wasInWeaponSwitch) {
            auto& component = entity->GetComponent();
            bool switchCondition = component.GetFrame() == component.GetFrameCount() - 1 && component.GetAccumulatedUpdates() >= component.GetUpdatesPerFrame() - 2;
            if (targetWeaponEquip == 0 && (playerAnimation == "Player_PistolIdle" || playerAnimation == "Player_PutAwayRifle" && switchCondition)) {
                entity->GetComponent().SwitchAnimation("Player_TakeOutPistol");
                equippedWeapon = 0;
                wasInWeaponSwitch = false;
            }
            if (targetWeaponEquip == 1 && (playerAnimation == "Player_RifleIdle" || playerAnimation == "Player_PutAwayPistol" && switchCondition)) {
                entity->GetComponent().SwitchAnimation("Player_TakeOutRifle");
                equippedWeapon = 1;
                wasInWeaponSwitch = false;
            }
        }

        // Weapon Switch Logic - Put Away Weapon
        if (game.Input.IsKeyPressedThisFrame(KeyCode::NumOne) && equippedWeapon != 0) {
            if (playerAnimation == "Player_RifleIdle") {
                entity->GetComponent().SwitchAnimation("Player_PutAwayRifle");
                entity->GetComponent().Update();
                entity->GetComponent().SetDefaultAnimation("Player_PistolIdle");
                targetWeaponEquip = 0;
                wasInWeaponSwitch = true;
            }
        }
        if (game.Input.IsKeyPressedThisFrame(KeyCode::NumTwo) && equippedWeapon != 1) {
            if (playerAnimation == "Player_PistolIdle") {
                entity->GetComponent().SwitchAnimation("Player_PutAwayPistol");
                entity->GetComponent().Update();
                entity->GetComponent().SetDefaultAnimation("Player_RifleIdle");
                targetWeaponEquip = 1;
                wasInWeaponSwitch = true;
            }
        }

        // Weapon Reload Logic
        if (game.Input.IsKeyPressedThisFrame(KeyCode::R)) {
            if (playerAnimation == "Player_PistolIdle" && currentPistolAmmo < maxPistolAmmo) {
                entity->GetComponent().SwitchAnimation("Player_PistolReload");
                wasReloadingPistol = true;
            }
            else if (playerAnimation == "Player_RifleIdle" && currentRifleAmmo < maxRifleAmmo && rifleAmmoPool > 0) {
                entity->GetComponent().SwitchAnimation("Player_RifleReload");
                wasReloadingRifle = true;
            }
        }

        // Makes the player face the mouse position
        std::pair<int, int> var = game.Input.GetMousePosition();
        Vector2 vect = Game::Vector2(var.first - 960.0, var.second - 540.0);
        double angle = vect.Angle();
        entity->GetComponent().SetDirection(angle);
	}

    void PlayerPseudoAI::OnDeath() {
        if (entity != nullptr) {
            AI::OnDeath();
            Globals::Audio().PlaySound("PlayerDeath");
            axe.QueueUnregisterFromGame();
            entity->GetComponent().SwitchAnimation("Player_Dead");
            entity->GetCollider().QueueUnregisterFromGame();
            entity->GetComponent().SetLayer(GraphicsEngine::CommonLayers::OnFloor);
        }
    } 
    
    void PlayerPseudoAI::OnAttackHit() {
        
    }

    void PlayerPseudoAI::OnHitByAttack(Actor* attacker, double damage) {
        if (entity != nullptr) {
            auto& stats = entity->GetStatsReference();
            if (stats.invulnerable || stats.currentInvincibilityFrames > 0 || damage < 0.0) {
                return;
            }

            bool shieldBlock = false;
            if (inShield) {
                auto attackAngle = (attacker->GetTransform().position - entity->GetTransform().position).Angle();
                auto angleDelta = abs(Math::GetAngleDifference(entity->GetTransform().direction, attackAngle));
                if (angleDelta < 100.0) {
                    shieldBlock = true;
                }
            }
            if (shieldBlock) {
                if (perfectGuardCounter > 0) {
                    damage *= 0.25;
                    hasPerfectGuarded = true;
                }
                else {
                    hasPerfectGuarded = false;
                }
                stats.currentInvincibilityFrames = 2;
                stats.shieldHealth -= damage;
                shieldRegenCounter = 0;
                if (stats.shieldHealth <= 0.0) {
                    stats.shieldHealth = 0.0;
                    shieldRegenCounter = -150;
                    stats.currentInvincibilityFrames = 40;
                    Globals::Audio().PlaySound("ShieldBreak");
                }
                else {
                    if (hasPerfectGuarded) {
                        Globals::Audio().PlaySound("PerfectGuard");
                    }
                    else {
                        Globals::Audio().PlaySound("ShieldImpact");
                    }
                }
            }
            else {
                stats.currentInvincibilityFrames = stats.onHitInvincibilityFrames;
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
                    }
                    else {
                        Globals::Audio().PlaySound("PlayerHurt");
                    }
                }
            }
            Globals::Audio().PlaySound("HurtBeta");
        }
    }

    void PlayerPseudoAI::GenericWeaponFireLogic(double damageToDeal, double angleDeltaToApply) {
        // Fires a non-piercing round that damages the first entity it hits, if it's an enemy

        auto mouse = Globals::Game().Input.GetMousePosition();
        auto shotAngle = (Game::Vector2(mouse.first, mouse.second) - Game::Vector2(960.0, 540.0)).Angle();
        auto results = Globals::Game().CreateRayCastHitList(entity->GetCollider().GetPosition(), Vector2::NormalVector(shotAngle + angleDeltaToApply) * 1800.0 + entity->GetCollider().GetPosition());
        
        double tracerDrawDistance = 1800;
        if (results.size() > 1) {
            // Collide with first result that is an "enemy" or a static collider
            Collider* hit = nullptr;
            double distance = 0.0;
            for (auto& elem : results) {
                auto entity = elem.second->GetEntity();
                if (elem.second->GetCombatLayer() == Collider::CombatLayer::Enemies || elem.second->GetColliderType() == Collider::ColliderType::Static) {
                    hit = elem.second;
                    distance = elem.first;
                    break;
                }
            }
            if (hit != nullptr) {
                auto entity = hit->GetEntity();
                if (dynamic_cast<Actor*>(entity) != nullptr) {
                    auto actor = (Actor*)entity;
                    auto actorAI = actor->GetAI();
                    if (actorAI != nullptr) {
                        this->OnAttackHit();
                        actorAI->OnHitByAttack(this->entity, damageToDeal);
                    }
                }
                tracerDrawDistance *= distance;
            }
            
        }
        Vector2 normal = Vector2::NormalVector(shotAngle + angleDeltaToApply);
        Vector2 perpend = Vector2::NormalVector(normal.Angle() + 90.0);
        Globals::Game().CreateDefaultTracerEffect(entity->GetCollider().GetPosition() + normal * 50 + perpend * 4, normal * tracerDrawDistance + entity->GetCollider().GetPosition() + perpend * 4 + normal * 3.0);
    }
}