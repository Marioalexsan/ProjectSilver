#include "PCHeader.h"
#include "Globals.h"
#include "FighterAI.h"
#include "Actor.h"

namespace Game {

    FighterAI::FighterAI() :
        strafesLeft(false),
        nextStrafeChange(300),
        nextShot(120),
        previousShot(-100),
        lastFramePlayerPos(Vector2::Zero),
        AI()
    {
    }

    void FighterAI::Update() {
        bool skipLogic = DelayedSpawningLogic();

        counter++;
        
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

        auto targetVector = player->GetTransform().position - this->entity->GetTransform().position;

        auto targetDirection = targetVector.Angle();
        auto entityDirection = this->entity->GetTransform().direction;

        auto angleDelta = abs(targetDirection - entityDirection);
        angleDelta = angleDelta > 180.0 ? 360.0 - angleDelta : angleDelta;

        int aimTime = (24 + (2 - Globals::Difficulty()) * 6);

        if (counter + aimTime < nextShot) {
            auto turnStrength = angleDelta > 8.0 ? 2.5 : 0.4;
            if (counter < previousShot + 10) {
                turnStrength /= 5;
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
        }

        

        auto distance = targetVector.Length();
        int forwardStrength = 4.0;
        int strafeStrength = 0.3;
        if (distance < 400.0) {
            forwardStrength = 0.0;
            strafeStrength = 3.3;
        }
        else if (distance < 600.0) {
            forwardStrength = (distance - 400.0) / 200.0 * 4.0;
            strafeStrength = 0.3 + (600.0 - distance) / 200.0 * 3.0;
        }

        double strafeAngle;
        if (strafesLeft) {
            strafeAngle = Utility::ScrollValue(entity->GetTransform().direction - 90.0, 0.0, 360.0);
        }
        else {
            strafeAngle = Utility::ScrollValue(entity->GetTransform().direction + 90.0, 0.0, 360.0);
        }

        double predictionStrength = 0.0;
        if (Globals::Difficulty() == GameMaster::DifficultyLevel::Hard) {
            predictionStrength = 0.15;
        }
        if (Globals::Difficulty() == GameMaster::DifficultyLevel::Insane) {
            predictionStrength = 0.30;
        }

        bool playerOutOfAim = (abs(targetDirection - entityDirection) > 30.0 || distance > 600.0);

        double shootDirection = 0.0;
        
        if (counter + aimTime > nextShot) {
            shootDirection = (targetVector + (player->GetTransform().position - lastFramePlayerPos) * predictionStrength * 60.0).Angle();
            
            double shootAngleDelta = abs(shootDirection - entityDirection);

            if (shootAngleDelta > 180.0) {
                if (shootDirection > 180.0) {
                    shootAngleDelta = -shootAngleDelta;
                }
            }
            else {
                if (shootDirection - entityDirection < 0.0) {
                    shootAngleDelta = -shootAngleDelta;
                }
            }

            if (!playerOutOfAim) {
                double amplifiedRatio = (double(nextShot) - counter) / aimTime;
                amplifiedRatio *= amplifiedRatio;

                forwardStrength *= (0.1 + 0.9 * amplifiedRatio);
                strafeStrength *= (0.1 + 0.9 * amplifiedRatio);
            }

            shootAngleDelta = Utility::ClampValue(shootAngleDelta, -18.0, 18.0);
            
            entity->GetTransform().direction += shootAngleDelta * 0.3;
            entity->GetTransform().direction = Utility::ScrollValue(entity->GetTransform().direction, 0.0, 360.0);
        }

        entity->Move(Vector2::NormalVector(strafeAngle) * strafeStrength);

        entity->MoveForward(forwardStrength);

        if (counter > nextShot) {
            if (playerOutOfAim) {
                nextShot += 6;
                return;
            }

            entity->GetComponent().SwitchAnimation("PlayerShoot");
            auto bulletID = Globals::Game().AddNewEnemy(EntityType::FighterBulletProjectile, entity->GetTransform().position + Vector2::NormalVector(entity->GetTransform().direction) * 40);
            auto theBullet = Globals::Game().GetEntity(bulletID);
            theBullet->GetTransform().direction = shootDirection;
            previousShot = nextShot;
            nextShot += 110 - 15 * Globals::Difficulty() + rand() % 20;
        }

        

        lastFramePlayerPos = player->GetTransform().position;
    }

    void FighterAI::OnDeath() {
        if (!EntityIsDeadAF()) {
            AI::OnDeath();
            entity->GetComponent().SwitchAnimation("CharDead");
            entity->GetCollider().QueueUnregisterFromGame();
            entity->GetComponent().SetLayer(GraphicsEngine::CommonLayers::OnFloor);
            destroyDelay = 300;
        }
    }

    void FighterAI::OnAttackHit() {

    }

    void FighterAI::OnHitByAttack(Actor* attacker, double damage) {
        if (!EntityIsDeadAF()) {
            AI::OnHitByAttack(attacker, damage);
        }
    }
}