#include "PCHeader.h"
#include "Globals.h"
#include "GunTurretAI.h"
#include "Actor.h"
#include "RifleAmmoPack.h"

namespace Game {

    GunTurretAI::GunTurretAI() :
        initiatedAttack(false),
        predictionStrengthToUse(0.0),
        burstDelay(0),
        ammo(3 + Globals::Difficulty()),
        previousShot(0),
        reloadCounter(0),
        waveBonus(false),
        nextShot(60),
        lastFramePlayerPos(Vector2::Zero),
        AI()
    {
    }

    void GunTurretAI::ApplyWaveEndBonus() {
        waveBonus = true;
    }

    void GunTurretAI::Update() {
        bool skipLogic = DelayedSpawningLogic();

        counter++;

        if (skipLogic) {
            return;
        }
        if (entity == nullptr) {
            return;
        }

        if (entity->GetStatsReference().isDead == true) {
            destroyDelay--;
            if (destroyDelay <= 150) {
                entity->GetComponent().SetAlpha(uint8_t(destroyDelay / 150.0 * 255.0));
            }
            if (destroyDelay == 0) {
                entity->SignalDestruction();
            }
            return;
        }

        int fullAmmo = 4 + Globals::Difficulty();
        int maxBurstDelay = 90 - Globals::Difficulty() * 15;
        reloadCounter++;
        if (reloadCounter > 150 - 15 * Globals::Difficulty()) {
            ammo = fullAmmo;
            reloadCounter = 0;
        }

        Entity* player = Globals::ThePlayer();

        Vector2 targetVector = player->GetTransform().position - this->entity->GetTransform().position;

        double targetDirection = targetVector.Angle();
        double entityDirection = this->entity->GetTransform().direction;

        double angleDelta = abs(targetDirection - entityDirection);
        angleDelta = angleDelta > 180.0 ? 360.0 - angleDelta : angleDelta;

        int aimTime = 12 - Globals::Difficulty() * 2;

        if (counter + aimTime < nextShot) {
            double turnStrength = angleDelta > 8.0 ? 2.5 : 0.4;
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



        double distance = targetVector.Length();

        bool playerOutOfAim = abs(targetDirection - entityDirection) > 30.0 || distance > 670.0;

        double shootDirection = 0.0;

        if (counter + aimTime == nextShot) {
            if (Globals::Difficulty() == GameMaster::DifficultyLevel::Normal) {
                predictionStrengthToUse = 0.05 + rand() % 20 / 100.0;
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

            shootAngleDelta = Utility::ClampValue(shootAngleDelta, -10.0, 10.0);

            entity->GetTransform().direction += shootAngleDelta * 0.3;
            entity->GetTransform().direction = Utility::ScrollValue(entity->GetTransform().direction, 0.0, 360.0);
        }

        if (counter > nextShot) {
            if (playerOutOfAim && !initiatedAttack || ammo == 0) {
                nextShot += 6;
                return;
            }

            reloadCounter = 0;

            if (burstDelay <= 0 && !initiatedAttack) {
                burstDelay = maxBurstDelay;
                initiatedAttack = true;
                Globals::Audio().PlaySound("TurretCharge");
            }

            if (burstDelay > 0) {
                nextShot += 6;
                burstDelay -= 6;
                return;
            }

            entity->GetComponent().SwitchAnimation("GunTurret_Shoot");
            uint64_t bulletID = Globals::Game().AddEntity(EntityType::FighterBulletProjectile, entity->GetTransform().position + Vector2::NormalVector(entity->GetTransform().direction) * 45);
            Entity* theBullet = Globals::Game().GetEntity(bulletID);
            theBullet->GetTransform().direction = entity->GetTransform().direction;
            previousShot = nextShot;
            nextShot += 26 - 6 * Globals::Difficulty() + rand() % 3;
            ammo--;

            if (ammo == 0) {
                initiatedAttack = false;
            }
        }


        lastFramePlayerPos = player->GetTransform().position;
    }

    void GunTurretAI::OnDeath() {
        if (!EntityIsDeadAF()) {
            AI::OnDeath();
            
            int ammo = 7 - Globals::Difficulty();
            if (!waveBonus) {
                entity->GetComponent().SwitchAnimation("GunTurret_Broken");
                Globals::Audio().PlaySound("TurretBreak");

                uint64_t ID = Globals::Game().AddEntity(EntityType::RifleAmmoPackThing, entity->GetTransform().position + Vector2(25.0, 0.0));
                RifleAmmoPack* pack = (RifleAmmoPack*)Globals::Game().GetEntity(ID);
                if (pack != nullptr) {
                    pack->SetAmmoToGrant(ammo);
                }
                uint64_t ID2 = Globals::Game().AddEntity(EntityType::RifleAmmoPackThing, entity->GetTransform().position + Vector2(-25.0, 0.0));
                RifleAmmoPack* pack2 = (RifleAmmoPack*)Globals::Game().GetEntity(ID2);
                if (pack2 != nullptr) {
                    pack2->SetAmmoToGrant(ammo);
                }
            }
            else {
                Globals::Audio().PlaySound("TurretDown");
                uint64_t ID = Globals::Game().AddEntity(EntityType::RifleAmmoPackThing, entity->GetTransform().position);
                RifleAmmoPack* pack = (RifleAmmoPack*)Globals::Game().GetEntity(ID);
                if (pack != nullptr) {
                    pack->SetAmmoToGrant(ammo);
                }
            }
            
            entity->GetCollider().QueueUnregisterFromGame();
            entity->GetComponent().SetLayer(GraphicsEngine::CommonLayers::OnFloor);
            destroyDelay = 300;
        }
    }

    void GunTurretAI::OnAttackHit() {

    }

    void GunTurretAI::OnHitByAttack(Actor* attacker, double damage) {
        if (!EntityIsDeadAF()) {
            Globals::Audio().PlaySound("HurtBeta");
            Globals::Audio().PlaySound("TurretHurt");
            AI::OnHitByAttack(attacker, damage);
        }
    }
}