#include "PCHeader.h"
#include "Globals.h"
#include "FighterAI.h"
#include "Actor.h"

namespace Game {

    FighterAI::FighterAI() :
        predictionStrengthToUse(0.0),
        aimAnnoyance(0),
        strafesLeft(false),
        nextStrafeChange(300),
        nextShot(110 - 15 * Globals::Difficulty() + rand() % 20),
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

        if (ProcessGenericDestroyDelay()) {
            return;
        }

        Entity* player = Globals::ThePlayer();

        Vector2 targetVector = player->GetTransform().position - this->entity->GetTransform().position;

        double targetDirection = targetVector.Angle();
        double entityDirection = this->entity->GetTransform().direction;

        double angleDelta = abs(targetDirection - entityDirection);
        angleDelta = angleDelta > 180.0 ? 360.0 - angleDelta : angleDelta;

        int aimTime = (24 + (2 - Globals::Difficulty()) * 6);

        if (counter + aimTime < nextShot) {
            double turnStrength = angleDelta > 8.0 ? 2.5 : 0.4;
            if (counter < previousShot + 10) {
                turnStrength /= 5;
            }
            entity->RotateTowardsDirection(turnStrength, targetDirection);
        }

        

        double distance = targetVector.Length();
        double forwardStrength = 4.0;
        double strafeStrength = 0.3;
        if (distance < 400.0) {
            forwardStrength = Utility::ClampValue(400.0 - distance, 0.0, 200.0) / 200.0 * (-2.4 - 0.8 * Globals::Difficulty());
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

        

        bool playerOutOfAim = (abs(targetDirection - entityDirection) > 30.0 || distance > 600.0);

        double shootDirection = 0.0;

        if (counter + aimTime == nextShot) {
            if (Globals::Difficulty() == GameMaster::DifficultyLevel::Normal) {
                predictionStrengthToUse = 0.15 + rand() % 15 / 100.0;
            }
            if (Globals::Difficulty() == GameMaster::DifficultyLevel::Hard) {
                predictionStrengthToUse = 0.3 + rand() % 15 / 100.0;
            }
            if (distance <= 300.0) {
                predictionStrengthToUse *= 0.25;
            }
            if (distance >= 520) {
                predictionStrengthToUse *= 1.2;
            }
        }

        
        
        if (counter + aimTime > nextShot) {
            shootDirection = (targetVector + (player->GetTransform().position - lastFramePlayerPos) * predictionStrengthToUse * 60.0).Angle();
            
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
            if (playerOutOfAim && aimAnnoyance <= 16) {
                nextShot += 6;
                aimAnnoyance++;
                return;
            }

            aimAnnoyance = 0;

            entity->GetComponent().SwitchAnimation("Player_PistolShoot");
            uint64_t bulletID = Globals::Game().AddEntity(EntityType::FighterBulletProjectile, entity->GetTransform().position + Vector2::NormalVector(entity->GetTransform().direction) * 40);
            Entity* theBullet = Globals::Game().GetEntity(bulletID);
            theBullet->GetTransform().direction = Utility::ScrollValue(entityDirection + (shootDirection - entityDirection) * 0.18, 0.0, 360.0);
            previousShot = nextShot;
            nextShot += 110 - 15 * Globals::Difficulty() + rand() % 20;
        }

        lastFramePlayerPos = player->GetTransform().position;
    }

    void FighterAI::OnDeath() {
        if (!EntityIsDeadAF()) {
            AI::OnDeath();
            Globals::Audio().PlaySound("Death");
            entity->GetComponent().SwitchAnimation("Player_Dead");
            entity->GetCollider().QueueUnregisterFromGame();
            entity->GetComponent().SetLayer(GraphicsEngine::CommonLayers::OnFloor);
            destroyDelay = 300;
        }
    }

    void FighterAI::OnAttackHit() {

    }

    void FighterAI::OnHitByAttack(Actor* attacker, double damage) {
        if (!EntityIsDeadAF()) {
            Globals::Audio().PlaySound("HurtBeta");
            AI::OnHitByAttack(attacker, damage);
        }
    }
}