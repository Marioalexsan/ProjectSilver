#include "PCHeader.h"
#include "Globals.h"
#include "ChaserAI.h"
#include "Actor.h"

namespace Game {

    ChaserAI::ChaserAI() :
        fastSwingCounter(60),
        currentSpeed(1.0),
        painCounter(100),
        postSwingDelay(0),
        strafesLeft(false),
        axe(Game::Vector2(0, 0), 50, Game::Collider::ColliderType::Combat),
        nextStrafeChange(300),
        lastFramePlayerPos(Vector2::Zero),
        doingSwing(false),
        AI()
    {
        axe.SetCombatDamage(31.0 + Globals::Difficulty() * 5.0);
        axe.SetCombatLayer(Collider::CombatLayer::None);
        axe.SetLayersToAttack({ Collider::CombatLayer::Players });
    }

    void ChaserAI::Update() {
        bool skipLogic = DelayedSpawningLogic();

        counter++;
        if (postSwingDelay > 0) {
            postSwingDelay--;
        }

        if (painCounter > 0) {
            painCounter--;
        }

        if (fastSwingCounter < 180) {
            fastSwingCounter++;
        }

        if (skipLogic) {
            return;
        }

        // Combat logic

        if (counter > nextStrafeChange) {
            strafesLeft = rand() % 2;
            nextStrafeChange += 200 - 20 * Globals::Difficulty() + rand() % 200;
        }

        if (entity == nullptr) {
            return;
        }

        if (entity->GetStatsReference().isDead == true) {
            destroyDelay--;
            if (destroyDelay <= 150) {
                entity->GetComponent().SetAlpha(destroyDelay / 150.0 * 255.0);
            }
            if (destroyDelay == 0) {
                entity->SignalDestruction();
            }
            return;
        }

        auto player = Globals::ThePlayer();

        double predictionStrength = 0.05;

        auto targetVector = player->GetTransform().position - this->entity->GetTransform().position + (player->GetTransform().position - lastFramePlayerPos) * predictionStrength * 60.0;

        auto targetDirection = targetVector.Angle();
        auto entityDirection = this->entity->GetTransform().direction;

        auto angleDelta = abs(targetDirection - entityDirection);
        angleDelta = angleDelta > 180.0 ? 360.0 - angleDelta : angleDelta;

        auto turnStrength = angleDelta > 8.0 ? 6.0 : 0.5;
        if (postSwingDelay > 60) {
            turnStrength /= 4;
        }
        else if (postSwingDelay > 0) {
            turnStrength /= 3 - 2 * (60.0 - postSwingDelay) / 60.0;
        }
        double maxSwingStrength = 1.2 * Globals::Difficulty() * 1.2;
        if (entity->GetComponent().GetCurrentAnimationID() == "Berserker_AxeSwing" && turnStrength > maxSwingStrength) {
            turnStrength = maxSwingStrength;
        }
        if (abs(targetDirection - entityDirection) > 180.0) {
            if (targetDirection > 180.0) {
                entity->GetTransform().direction -= turnStrength;
            }
            else {
                entity->GetTransform().direction += turnStrength;
            }
        }
        else {
            if (targetDirection - entityDirection > 0) {
                entity->GetTransform().direction += turnStrength;
            }
            else {
                entity->GetTransform().direction -= turnStrength;
            }
        }
        entity->GetTransform().direction = Utility::ScrollValue(entity->GetTransform().direction, 0.0, 360.0);

        auto distance = targetVector.Length();
        int forwardStrength = 4.0 + Globals::Difficulty() * 0.7;
        int strafeStrength = 0.0;
        if (distance < 600.0) {
            strafeStrength = Utility::ClampValue(600.0 - distance, 0.0, 400.0) / 400.0 * 2.8;
        }

        else if (distance > 900.0) {
            forwardStrength += Utility::ClampValue(distance - 900.0, 0.0, 200.0) / 200.0 * 2.0;
        }

        double strafeAngle;
        if (strafesLeft) {
            strafeAngle = Utility::ScrollValue(entity->GetTransform().direction - 90.0, 0.0, 360.0);
        }
        else {
            strafeAngle = Utility::ScrollValue(entity->GetTransform().direction + 90.0, 0.0, 360.0);
        }

        if (entity->GetComponent().GetCurrentAnimationID() == "Berserker_AxeSwing") {
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
            else if (currentFrame <= 10) {
                factor = 2.0 * Utility::ClampValue(11 - currentFrame, 0, 3) / 3;
            }
            else {
                factor = 2.0 * Utility::ClampValue(currentFrame - 10, 0, 3) / 3;
            }
            forwardStrength *= factor * 0.9;
        }
        else if (postSwingDelay > 60) {
            strafeStrength /= 8;
            forwardStrength /= 8;
        }
        else if (postSwingDelay > 0) {
            double factor = (60.0 - postSwingDelay) / 60.0;
            strafeStrength /= 8 - 7 * factor;
            forwardStrength /= 8 - 7 * factor;
        }

        double changeFactor = 1.0;
        double targetSpeed;
        if (painCounter > 180) {
            targetSpeed = 0.5;
        }
        else if (painCounter > 60.0) {
            targetSpeed = 0.65;
        }
        else if (painCounter > 30.0) {
            targetSpeed = 0.8;
        }
        else if (painCounter > 0.0) {
            targetSpeed = 1.0;
        }
        else {
            targetSpeed = 1.6;
            changeFactor = 0.25;
        }

        currentSpeed = currentSpeed + (targetSpeed - currentSpeed) * 0.07 * changeFactor;

        double speedToUse = currentSpeed;
        if (entity->GetComponent().GetCurrentAnimationID() == "Berserker_AxeSwing") {
            speedToUse = sqrt(speedToUse);
        }

        entity->Move(Vector2::NormalVector(strafeAngle) * strafeStrength * speedToUse);
        entity->MoveForward(forwardStrength * speedToUse);

        if (entity->GetComponent().GetCurrentAnimationID() != "Berserker_AxeSwing" && distance < 200 && angleDelta < 40.0 && postSwingDelay < 20) {
            entity->GetComponent().SwitchAnimation("Berserker_AxeSwing");
        }

        if (entity->GetComponent().GetCurrentAnimationID() == "Berserker_AxeSwing") {
            axe.SetPosition(entity->GetTransform().position + Vector2::NormalVector(entity->GetTransform().direction) * 20);

            if (entity->GetComponent().GetFrame() == 5 && !doingSwing) {
                doingSwing = true;
                axe.SetOwner(entity);
                axe.RegisterToGame();
            }

            if (entity->GetComponent().GetFrame() == 8) {
                if (doingSwing) {
                    postSwingDelay = 110 - Globals::Difficulty() * 10;
                    if (fastSwingCounter >= 165) {
                        fastSwingCounter -= 165;
                        postSwingDelay = 35 - Globals::Difficulty() * 15;
                    }
                }
                
                doingSwing = false;
                axe.UnregisterFromGame();
            }
        }

        lastFramePlayerPos = player->GetTransform().position;
    }

    void ChaserAI::OnDeath() {
        if (!EntityIsDeadAF()) {
            AI::OnDeath();
            Globals::Audio().PlaySound("Death");
            entity->GetComponent().SwitchAnimation("Berserker_Dead");
            entity->GetCollider().QueueUnregisterFromGame();
            axe.QueueUnregisterFromGame();
            entity->GetComponent().SetLayer(GraphicsEngine::CommonLayers::OnFloor);
            destroyDelay = 300;
        }
    }

    void ChaserAI::OnAttackHit() {
        axe.QueueUnregisterFromGame();
    }

    void ChaserAI::OnHitByAttack(Actor* attacker, double damage) {
        if (!EntityIsDeadAF()) {
            painCounter += int(damage * (3 - 0.4 * Globals::Difficulty()));
            if (painCounter < 120.0) {
                painCounter = 120.0;
            }
            Globals::Audio().PlaySound("HurtBeta");
            Globals::Audio().PlaySound("ZerkHurt");
            AI::OnHitByAttack(attacker, damage);
        }
    }
}