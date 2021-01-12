#include "PCHeader.h"
#include "Globals.h"
#include "KnightAI.h"
#include "Actor.h"

namespace Game {

    KnightAI::KnightAI() :
        postSwingDelay(0),
        strafesLeft(false),
        sword(Game::Vector2(0, 0), 50, Game::Collider::ColliderType::Combat),
        nextStrafeChange(300),
        lastFramePlayerPos(Vector2::Zero),
        doingSwing(false),
        AI()
    {
        sword.SetCombatDamage(28.0 + Globals::Difficulty() * 5.0);
        sword.SetCombatLayer(Collider::CombatLayer::None);
        sword.SetLayersToAttack({ Collider::CombatLayer::Players });
        sword.SetCollisionOptions({Collider::CollisionOptions::DoNotHitRememberedEntities});
    }

    void KnightAI::Update() {
        bool skipLogic = DelayedSpawningLogic();

        counter++;
        if (postSwingDelay > 0) {
            postSwingDelay--;
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

        double predictionStrength = 0.0;
        if (Globals::Difficulty() == GameMaster::DifficultyLevel::Normal) {
            predictionStrength = 0.1;
        }
        if (Globals::Difficulty() == GameMaster::DifficultyLevel::Hard) {
            predictionStrength = 0.2;
        }

        auto targetVector = player->GetTransform().position - this->entity->GetTransform().position + (player->GetTransform().position - lastFramePlayerPos) * predictionStrength * 60.0;

        auto targetDirection = targetVector.Angle();
        auto entityDirection = this->entity->GetTransform().direction;

        auto angleDelta = abs(targetDirection - entityDirection);
        angleDelta = angleDelta > 180.0 ? 360.0 - angleDelta : angleDelta;

        auto turnStrength = angleDelta > 8.0 ? 5.0 : 0.5;
        if (postSwingDelay > 0) {
            turnStrength /= 4;
            turnStrength /= 2.0 - Utility::ClampValue(60.0 - postSwingDelay, 0.0, 60.0) / 60.0 + 1.0;
        }
        double maxSwingStrength = Globals::Difficulty() + 1.0;
        if (entity->GetComponent().GetCurrentAnimationID() == "Knight_Swing" && turnStrength > maxSwingStrength) {
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
        int forwardStrength = 4.0 + Globals::Difficulty();
        int strafeStrength = 0.0;
        if (distance < 600.0) {
            forwardStrength += Utility::ClampValue(distance - 600.0, -400.0, 0.0) / 400.0 * (1 - 0.4 * Globals::Difficulty());
            strafeStrength = Utility::ClampValue(600.0 - distance, 0.0, 400.0) / 400.0 * 2.8;
        }
        if (distance < 300.0) {
            strafeStrength *= (300.0 - distance) / 300.0;
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

        if (entity->GetComponent().GetCurrentAnimationID() == "Knight_Swing") {
            int currentFrame = entity->GetComponent().GetFrame();
            double factor;
            if (currentFrame <= 3) {
                factor = Utility::ClampValue(6 - currentFrame, 0, 6) / 6;
            }
            else if (currentFrame <= 8) {
                factor = Utility::ClampValue(currentFrame - 1, 0, 6) / 4;
            }
            else {
                factor = 1;
            }
            strafeStrength *= 0.7 * factor + 0.3;
            forwardStrength *= 0.7 * factor + 0.3;
        }
        else if (postSwingDelay > 60) {
            strafeStrength /= 4;
            forwardStrength /= 4;
        }
        else if (postSwingDelay > 0) {
            double factor = (60.0 - postSwingDelay) / 60.0;
            strafeStrength /= 4 - 3 * factor;
            forwardStrength /= 4 - 3 * factor;
        }

        entity->Move(Vector2::NormalVector(strafeAngle) * strafeStrength);
        entity->MoveForward(forwardStrength);

        if (entity->GetComponent().GetCurrentAnimationID() != "Knight_Swing" && distance < 200 && angleDelta < 40.0 && postSwingDelay < 100) {
            entity->GetComponent().SwitchAnimation("Knight_Swing");
        }

        if (entity->GetComponent().GetCurrentAnimationID() == "Knight_Swing") {
            sword.SetPosition(entity->GetTransform().position + Vector2::NormalVector(entity->GetTransform().direction) * 40);
        
            if (entity->GetComponent().GetFrame() == 7 && !doingSwing) {
                doingSwing = true;
                sword.SetOwner(entity);
                sword.GetHitList().clear();
                sword.RegisterToGame();
            }

            if (entity->GetComponent().GetFrame() == 10) {
                doingSwing = false;
                sword.UnregisterFromGame();
                postSwingDelay = 120 - Globals::Difficulty() * 32;
            }
        }

        lastFramePlayerPos = player->GetTransform().position;
    }

    void KnightAI::OnDeath() {
        if (!EntityIsDeadAF()) {
            AI::OnDeath();
            Globals::Audio().PlaySound("Death");
            entity->GetComponent().SwitchAnimation("Knight_Dead");
            entity->GetCollider().QueueUnregisterFromGame();
            sword.QueueUnregisterFromGame();
            entity->GetComponent().SetLayer(GraphicsEngine::CommonLayers::OnFloor);
            destroyDelay = 300;
        }
    }

    void KnightAI::OnAttackHit() {
        sword.QueueUnregisterFromGame();
    }

    void KnightAI::OnHitByAttack(Actor* attacker, double damage) {
        if (!EntityIsDeadAF()) {
            auto attackAngle = (attacker->GetTransform().position - entity->GetTransform().position).Angle();
            auto angleDelta = abs(attackAngle - entity->GetTransform().direction);
            angleDelta = angleDelta > 180.0 ? 360.0 - angleDelta : angleDelta;
            if (angleDelta < 90.0) {
                damage *= 0.2;
                Globals::Audio().PlaySound("ShieldImpact");
            }
            Globals::Audio().PlaySound("HurtBeta");
            AI::OnHitByAttack(attacker, damage);
        }
    }
}