#include <ProjectSilver/Actor.hpp>
#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/KnightAI.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{

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
        sword.SetLayersToAttack({Collider::CombatLayer::Players});
        sword.SetCollisionOptions({Collider::CollisionOptions::DoNotHitRememberedEntities});
    }

    void KnightAI::Update()
    {
        bool skipLogic = DelayedSpawningLogic();

        counter++;
        if (postSwingDelay > 0)
        {
            postSwingDelay--;
        }

        if (skipLogic)
        {
            return;
        }

        // Combat logic

        if (counter > nextStrafeChange)
        {
            strafesLeft = rand() % 2;
            nextStrafeChange += 200 - 20 * Globals::Difficulty() + rand() % 200;
        }

        if (entity == nullptr)
        {
            return;
        }

        if (ProcessGenericDestroyDelay())
        {
            return;
        }

        Entity* player = Globals::ThePlayer();

        double predictionStrength = 0.05;

        Vector2 targetVector = player->GetTransform().position -
                               this->entity->GetTransform().position +
                               (player->GetTransform().position - lastFramePlayerPos) *
                                   predictionStrength * 60.0;

        double targetDirection = targetVector.Angle();
        double entityDirection = this->entity->GetTransform().direction;

        double angleDelta = abs(targetDirection - entityDirection);
        angleDelta        = angleDelta > 180.0 ? 360.0 - angleDelta : angleDelta;

        double turnStrength = angleDelta > 8.0 ? 5.0 : 0.5;
        if (postSwingDelay > 55)
        {
            turnStrength /= 6;
        }
        double maxSwingStrength = 0.3 * Globals::Difficulty() + 0.6;
        if (entity->GetComponent().GetCurrentAnimationID() == "Knight_Swing" &&
            turnStrength > maxSwingStrength)
        {
            if (entity->GetComponent().GetFrame() <= 2 * Globals::Difficulty())
            {
                maxSwingStrength *= 3;
            }
            turnStrength = maxSwingStrength;
        }

        entity->RotateTowardsDirection(turnStrength, targetDirection);

        double distance        = targetVector.Length();
        double forwardStrength = 4.0 + Globals::Difficulty();
        double strafeStrength  = 0.0;
        if (distance < 600.0)
        {
            forwardStrength += Utility::ClampValue(distance - 600.0, -400.0, 0.0) /
                               400.0 * (1 - 0.4 * Globals::Difficulty());
            strafeStrength = Utility::ClampValue(600.0 - distance, 0.0, 400.0) / 400.0 * 2.8;
        }
        if (distance < 300.0)
        {
            strafeStrength *= (300.0 - distance) / 300.0;
        }

        else if (distance > 900.0)
        {
            forwardStrength += Utility::ClampValue(distance - 900.0, 0.0, 200.0) / 200.0 * 2.0;
        }

        double strafeAngle;
        if (strafesLeft)
        {
            strafeAngle = Utility::ScrollValue(entity->GetTransform().direction - 90.0, 0.0, 360.0);
        }
        else
        {
            strafeAngle = Utility::ScrollValue(entity->GetTransform().direction + 90.0, 0.0, 360.0);
        }

        if (entity->GetComponent().GetCurrentAnimationID() == "Knight_Swing")
        {
            int    currentFrame = entity->GetComponent().GetFrame();
            double factor;
            if (currentFrame <= 3)
            {
                factor = Utility::ClampValue(6 - currentFrame, 0, 6) / 6;
            }
            else if (currentFrame <= 8)
            {
                factor = Utility::ClampValue(currentFrame - 1, 0, 6) / 4;
            }
            else
            {
                factor = 1;
            }
            strafeStrength *= (0.7 * factor + 0.3) * 1.2;
            forwardStrength *= (0.7 * factor + 0.3) * 1.2;
        }
        else
        {
            if (postSwingDelay > 20)
            {
                forwardStrength /= 16;
                strafeStrength /= 16;
            }
            else if (postSwingDelay > 0)
            {
                forwardStrength /= 16 - 15 * (20.0 - postSwingDelay) / 20.0;
                strafeStrength /= 16 - 15 * (20.0 - postSwingDelay) / 20.0;
            }
        }

        entity->Move(Vector2::NormalVector(strafeAngle) * strafeStrength);
        entity->MoveForward(forwardStrength);

        if (entity->GetComponent().GetCurrentAnimationID() != "Knight_Swing" &&
            distance < 200 && angleDelta < 40.0 &&
            postSwingDelay < 50 + 15 * Globals::Difficulty())
        {
            entity->GetComponent().SwitchAnimation("Knight_Swing");
        }

        if (entity->GetComponent().GetCurrentAnimationID() == "Knight_Swing")
        {
            sword.SetPosition(entity->GetTransform().position +
                              Vector2::NormalVector(entity->GetTransform().direction) * 40);

            if (entity->GetComponent().GetFrame() == 7 && !doingSwing)
            {
                doingSwing = true;
                sword.SetOwner(entity);
                sword.GetHitList().clear();
                sword.RegisterToGame();
            }

            if (entity->GetComponent().GetFrame() == 10)
            {
                doingSwing = false;
                sword.UnregisterFromGame();
                postSwingDelay = 140 - Globals::Difficulty() * 20;
            }
        }

        lastFramePlayerPos = player->GetTransform().position;
    }

    void KnightAI::OnDeath()
    {
        if (!EntityIsDeadAF())
        {
            AI::OnDeath();
            Globals::Audio().PlaySound("Death");
            entity->GetComponent().SwitchAnimation("Knight_Dead");
            entity->GetCollider().QueueUnregisterFromGame();
            sword.QueueUnregisterFromGame();
            entity->GetComponent().SetLayer(GraphicsEngine::CommonLayers::OnFloor);
            destroyDelay = 300;
        }
    }

    void KnightAI::OnAttackHit()
    {
        sword.QueueUnregisterFromGame();
    }

    void KnightAI::OnHitByAttack(Actor* attacker, double damage)
    {
        if (!EntityIsDeadAF())
        {
            double attackAngle = (attacker->GetTransform().position -
                                  entity->GetTransform().position)
                                     .Angle();
            double angleDelta = abs(
                Math::GetAngleDifference(entity->GetTransform().direction, attackAngle));

            if (angleDelta < 80.0)
            {
                if (damage > 50.0)
                {
                    // Nerfs high damage attacks
                    damage -= (damage - 50.0) * 0.5;
                }
                damage *= 0.35;
                damage = damage < 0.0 ? 0.0 : damage;
                Globals::Audio().PlaySound("ShieldImpact");
            }
            Globals::Audio().PlaySound("HurtBeta");
            AI::OnHitByAttack(attacker, damage);
        }
    }
} // namespace Game