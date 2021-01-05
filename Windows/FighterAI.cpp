#include "PCHeader.h"
#include "Globals.h"
#include "FighterAI.h"
#include "Actor.h"

namespace Game {

    FighterAI::FighterAI() :
        AI()
    {}

    void FighterAI::Update() {
        counter++;

        if (counter % 375 == 0) {
            strafesLeft = rand() % 2;
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

        auto turnStrength = angleDelta > 8.0 ? 2.5 : 0.4;
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
        entity->Move(Vector2::NormalVector(strafeAngle) * strafeStrength);

        entity->MoveForward(forwardStrength);

        if (counter % 120 == 0) {
            if (abs(targetDirection - entityDirection) > 30.0) {
                counter -= 30;
                return;
            }
            entity->GetComponent().SwitchAnimation("PlayerShoot");
            auto bulletID = Globals::Game().AddNewEnemy(ActorType::FighterBulletProjectile, entity->GetTransform().position + Vector2::NormalVector(entity->GetTransform().direction) * 40);
            auto theBullet = Globals::Game().GetEntity(bulletID);
            theBullet->GetTransform().direction = entity->GetTransform().direction;
        }
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