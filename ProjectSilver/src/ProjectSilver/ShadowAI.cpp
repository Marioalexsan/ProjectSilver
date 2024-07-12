#include <ProjectSilver/Actor.hpp>
#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/ShadowAI.hpp>

namespace Game
{

    // ULTRA SUPER DUPER UNIMPLEMENTED
    // FOR ALL PURPOSES, THIS FILE IS BROKEN AND SHOULD NOT BE USED (until repaired)

    ShadowAI::ShadowAI() :
    lastStand(false),
    strafesLeft(false),
    nextStrafeChange(300),
    doingSwing(false),
    postSwingDelay(0),
    swingCount(0),
    pistolCredits(0),
    pistolShotCount(0),
    pistolUseCooldown(0),
    nextShot(0),
    hurtEvasionCredits(0),
    hurtEvasionTriggerDelay(0),
    hurtEvasionCooldown(0),
    painCounter(100),
    currentSpeed(1.0),
    specialHurtSoundCooldown(0),
    action(Actions::MeleeChase),
    axe(Game::Vector2(0, 0), 60, Game::Collider::ColliderType::Combat),
    AI(),
    lastFramePlayerPos(0.0, 0.0),
    seesPlayer(true),
    hitRecently(true)
    {
        axe.SetCombatDamage(17.0 + 5.0 * Globals::Difficulty());
        axe.SetCombatLayer(Collider::CombatLayer::None);
        axe.SetLayersToAttack({Collider::CombatLayer::Players});
        axe.SetCollisionOptions({Collider::CollisionOptions::DoNotHitRememberedEntities});

        hp.SetTexture("ShadowHealthBar");
        hp.SetHPTexture("ShadowHealth");
        hp.SetLastStandHPTexture("ShadowMaxHealth");
        hp.SetLayer(GraphicsEngine::CommonLayers::BelowGUI);

        hp.SetCenter({400, 20});
        hp.SetPosition({960, 1020});
        hp.SetAlpha(0);
        hp.SetRelativeToCamera(false);
        hp.RegisterToGame();
    }

    void ShadowAI::DecideAction()
    {
        const std::string& shadowAnimation = entity->GetComponent().GetCurrentAnimationID();
        auto& game   = Globals::Game();
        auto  player = Globals::ThePlayer();


        auto distance = (player->GetTransform().position - entity->GetTransform().position)
                            .Length();

        if (distance < 300.0)
        {
        }
        else
        {
        }
    }

    ShadowAI::~ShadowAI()
    {
        hp.UnregisterFromGame();
    }


    int ShadowAI::GetDifficultyLevel()
    {
        int   difficulty = Globals::Difficulty() * 2;
        auto& stats      = entity->GetStatsReference();

        double healthRatio = stats.health / stats.maxHealth;

        if (healthRatio < 0.8)
        {
            difficulty++;
        }
        if (healthRatio < 0.5)
        {
            difficulty++;
        }
        if (healthRatio < 0.3)
        {
            difficulty++;
        }

        if (lastStand)
        {
            difficulty += 5;

            if (healthRatio < 0.4 + Globals::Difficulty() * 0.15)
            {
                difficulty++;
            }
        }

        return difficulty;
    }

    void ShadowAI::CounterLogic()
    {
        counter++;

        if (painCounter > 0)
        {
            painCounter--;
            if (painCounter < 0)
            {
                painCounter = 0;
            }
        }

        if (postSwingDelay > 0)
        {
            postSwingDelay--;
        }

        if (pistolUseCooldown > 0)
        {
            pistolUseCooldown--;
        }

        if (specialHurtSoundCooldown > 0)
        {
            specialHurtSoundCooldown--;
        }

        if (counter > nextStrafeChange)
        {
            strafesLeft = rand() % 2;
            nextStrafeChange += 200 - 20 * Globals::Difficulty() + rand() % 200;
        }
    }

    double ShadowAI::CalculateSwingFactor()
    {
        double swingFactor  = 1.0;
        int    currentFrame = entity->GetComponent().GetFrame();

        if (currentFrame < 3)
        {
            swingFactor = 0.2;
        }
        else if (4 <= currentFrame && currentFrame <= 6)
        {
            swingFactor = 2.3 * Utility::ClampValue(currentFrame - 3, 0, 3) / 3;
        }
        else if (currentFrame <= 8)
        {
            swingFactor = 2.3;
        }
        else if (currentFrame <= 10)
        {
            swingFactor = 2.0 * Utility::ClampValue(11 - currentFrame, 0, 3) / 3;
        }
        else
        {
            swingFactor = 2.0 * Utility::ClampValue(currentFrame - 10, 0, 3) / 3;
        }

        if (action == Actions::MeleeLungeHitCombo)
        {
            swingFactor *= 1.1;
        }

        return swingFactor;
    }

    void ShadowAI::SwingAxe()
    {
        axe.SetPosition(entity->GetTransform().position +
                        Vector2::NormalVector(entity->GetTransform().direction) * 20);
        int currentDifficulty = GetDifficultyLevel();

        if (entity->GetComponent().GetFrame() == 5 && !doingSwing)
        {
            doingSwing = true;
            axe.SetOwner(entity);
            axe.GetHitList().clear();
            axe.RegisterToGame();
        }

        if (rand() % Utility::ClampValue(8 - GetDifficultyLevel() / 2, 1, 10) == 0)
        {
            if (action == Actions::MeleeLungeHitCombo)
            {
                action = Actions::MeleeTurnHitCombo;
            }
            else if (action == Actions::MeleeTurnHitCombo)
            {
                action = Actions::MeleeLungeHitCombo;
            }
        }

        if (entity->GetComponent().GetFrame() == 8)
        {
            if (doingSwing)
            {
                if (swingCount == 0)
                {
                    postSwingDelay = 170 - currentDifficulty * 3;
                    action         = Actions::MeleeChase;
                }
                else
                {
                    postSwingDelay = 42 - currentDifficulty;
                }
            }

            doingSwing = false;
            axe.UnregisterFromGame();
        }
    }

    void ShadowAI::Update()
    {
        bool skipLogic = DelayedSpawningLogic();

        CounterLogic();

        if (skipLogic || entity == nullptr)
        {
            return;
        }

        if (EntityIsDeadAF())
        {
            if (hp.GetAlpha() > 0)
            {
                hp.SetAlpha(Utility::ClampValue(hp.GetAlpha() - 4, 0, 255));
            }
            return;
        }

        if (hp.GetAlpha() < 255)
        {
            hp.SetAlpha(Utility::ClampValue(hp.GetAlpha() + 5, 0, 255));
        }
        auto&  stats = entity->GetStatsReference();
        double fHP   = stats.health;
        double fLHP  = stats.health;
        if (lastStand)
        {
            fHP = 0.0;
        }
        else
        {
            fLHP = stats.maxHealth;
        }
        hp.SetHPValues(fHP, fLHP, stats.maxHealth);

        int     currentDifficulty = GetDifficultyLevel();
        Entity* player            = Globals::ThePlayer();

        double movePredictionStrength = 0.02;

        if (counter % 7 == 0)
        {
            seesPlayer = HasPlayerLineOfSight();
        }

        if (action != Actions::PistolRetaliation)
        {
            pistolCredits++;
            if (lastStand && counter % 2 == 0)
            {
                pistolCredits++;
            }
            if (painCounter <= 10.0 && counter % 3 == 0)
            {
                pistolCredits++;
            }
        }


        if (action != Actions::Recovery)
        {
            if (painCounter >= 100)
            {
                painCounter--;
            }
            if (painCounter >= 200)
            {
                painCounter--;
            }
        }

        if (action == Actions::Recovery && painCounter <= 0 &&
            entity->GetComponent().GetCurrentAnimationID() == "Shadow_Recovery")
        {
            entity->GetComponent().SwitchAnimation("Shadow_RecoveryEnd");
            entity->GetComponent().SetDefaultAnimation("Shadow_AxeIdle");
        }

        if (action == Actions::Recovery &&
            entity->GetComponent().GetCurrentAnimationID() == "Shadow_AxeIdle")
        {
            action = Actions::MeleeChase;
        }

        int pistolCost = 1450;

        if (pistolUseCooldown == 0 && pistolCredits >= pistolCost &&
            entity->GetComponent().GetCurrentAnimationID() == "Shadow_AxeIdle")
        {
            action = Actions::PistolRetaliation;
            pistolCredits -= pistolCost;
            entity->GetComponent().SetDefaultAnimation("Shadow_PistolIdle");
            entity->GetComponent().SwitchAnimation("Shadow_TakeOutPistol");
            pistolShotCount = 3 + (currentDifficulty >= 3 ? 1 : 0) +
                              (currentDifficulty >= 6 ? 1 : 0) +
                              (currentDifficulty >= 10 ? 1 : 0) +
                              (currentDifficulty >= 13 ? 1 : 0);
            nextShot = counter + Utility::ClampValue(50 - currentDifficulty * 2, 15, 60);
        }
        else if (pistolShotCount == 0 &&
                 entity->GetComponent().GetCurrentAnimationID() == "Shadow_PistolIdle")
        {
            action = Actions::MeleeChase;
            entity->GetComponent().SetDefaultAnimation("Shadow_AxeIdle");
            entity->GetComponent().SwitchAnimation("Shadow_PutAwayPistol");
            pistolUseCooldown = 340 - 15 * currentDifficulty;
        }


        // Targeting parameters
        Vector2 moveTargetVector = player->GetTransform().position -
                                   this->entity->GetTransform().position +
                                   (player->GetTransform().position - lastFramePlayerPos) *
                                       movePredictionStrength * 60.0;
        double  moveTargetDirection = moveTargetVector.Angle();
        Vector2 trueTargetVector    = player->GetTransform().position -
                                   this->entity->GetTransform().position;
        double trueTargetDirection = trueTargetVector.Angle();
        double distance            = moveTargetVector.Length();
        double angleDelta          = abs(
            Math::GetAngleDifference(entity->GetTransform().direction, moveTargetDirection));

        // Decide turn strength
        double turnStrength = angleDelta > 8.0 ? (6.0 + 0.1 * currentDifficulty) : 0.5;
        if (entity->GetComponent().GetCurrentAnimationID() == "Shadow_AxeIdle")
        {
            if (postSwingDelay > 130)
            {
                turnStrength /= 16;
            }
            else if (postSwingDelay > 0)
            {
                turnStrength /= 2;
            }
        }

        if (action == Actions::Recovery)
        {
            turnStrength = 0;
        }

        double maxSwingStrength = 1.4;
        if (action == Actions::MeleeTurnHitCombo)
        {
            maxSwingStrength = 3.5 + currentDifficulty / 6.0;
        }


        if (entity->GetComponent().GetCurrentAnimationID() == "Shadow_AxeSwing" &&
            turnStrength > maxSwingStrength)
        {
            turnStrength = maxSwingStrength;
        }

        entity->RotateTowardsDirection(turnStrength, moveTargetDirection);

        double maxForwardStrength  = 4.8 + 15.0 / 12.0;
        double baseForwardStrength = 4.8 + currentDifficulty / 12.0;
        double forwardStrength     = baseForwardStrength;
        if (painCounter <= 40)
        {
            forwardStrength *= 1.1;
        }
        else if (painCounter >= 120)
        {
            forwardStrength *= 0.84;
        }
        else if (painCounter >= 70)
        {
            forwardStrength *= 0.9;
        }

        double strafeStrength = 0.0;


        if ((lastStand || entity->GetComponent().GetCurrentAnimationID() == "Shadow_"
                                                                            "AxeSwing") &&
            counter % 5 == 0)
        {
            int frame = entity->GetComponent().GetFrame();
            if (frame == -1)
            {
                frame = 0;
            }
            Game::Transform tform = entity->GetTransform();
            tform.position += Vector2::NormalVector(entity->GetTransform().direction) *
                                  -1 * (rand() % 6) +
                              Vector2::NormalVector(rand() % 360) * 4;
            Globals::Game().CreateAfterImageEffect(tform,
                                                   entity->GetComponent().GetCurrentAnimationID(),
                                                   frame);
        }

        // Movement logic related switch
        switch (action)
        {
            case Actions::MeleeChase:
            {
                if (distance > 800.0)
                {
                    strafeStrength = 0.1;
                    forwardStrength *= 1.3;
                }
                if (distance >= 400 && distance <= 800)
                {
                    strafeStrength = 3.5 - 3.4 * (800.0 - distance) / 400.0;
                    forwardStrength *= 1.3 -
                                       0.3 * Utility::ClampValue((800.0 - distance) / 400.0,
                                                                 0.0,
                                                                 1.0);
                }
                if (distance < 400)
                {
                    strafeStrength = 3.5;
                }
                if (entity->GetComponent().GetCurrentAnimationID() == "Shadow_AxeIdle")
                {
                    if (postSwingDelay > 80)
                    {
                        forwardStrength /= 16;
                        strafeStrength /= 16;
                    }
                    else if (postSwingDelay > 0)
                    {
                        forwardStrength /= 16 - 15 * (80.0 - postSwingDelay) / 80.0;
                        strafeStrength /= 16 - 15 * (80.0 - postSwingDelay) / 80.0;
                    }
                }
            }
            break;
            case Actions::MeleeLungeHitCombo:
            {
                forwardStrength *= 1.25;
                strafeStrength *= 0.25;
            }
            break;
            case Actions::PistolRetaliation:
            {
                if (distance > 600)
                {
                    forwardStrength *= 1.0 + 0.3 * Utility::ClampValue((distance - 600) / 200.0,
                                                                       0.0,
                                                                       1.0);
                }
                else if (distance > 450)
                {
                    forwardStrength *= 1.0 -
                                       Utility::ClampValue((600 - distance) / 150.0, 0.0, 1.0);
                    strafeStrength *= 1.1;
                }
                else
                {
                    forwardStrength *= -0.6 *
                                       Utility::ClampValue((450 - distance) / 450.0, 0.0, 1.0);
                    strafeStrength *= 1.3;
                }
            }
            break;
        }

        if (entity->GetComponent().GetCurrentAnimationID() == "Shadow_AxeSwing")
        {
            double swingFactor = CalculateSwingFactor();

            forwardStrength *= swingFactor * 0.9;
            strafeStrength /= 8;
        }

        double targetSpeed = 1.0;
        if (action == Actions::Recovery)
        {
            targetSpeed = 0;
        }

        currentSpeed = currentSpeed + (targetSpeed - currentSpeed) * 0.07;

        forwardStrength *= currentSpeed;
        strafeStrength *= currentSpeed;

        double strafeAngle;
        if (strafesLeft)
        {
            strafeAngle = Utility::ScrollValue(entity->GetTransform().direction - 90.0, 0.0, 360.0);
        }
        else
        {
            strafeAngle = Utility::ScrollValue(entity->GetTransform().direction + 90.0, 0.0, 360.0);
        }

        if (!seesPlayer && action != Actions::Recovery)
        {
            strafeStrength += 1.5;
            if (action == Actions::PistolRetaliation)
            {
                strafeStrength += 1.4;
            }
            strafeStrength *= 1.2;
        }

        entity->Move(Vector2::NormalVector(strafeAngle) * strafeStrength);
        entity->MoveForward(forwardStrength);

        bool playerOutOfPistolRange = distance > 750.0 || (!seesPlayer && !hitRecently);


        // Action logic related switch
        switch (action)
        {
            case Actions::PistolRetaliation:
            {
            }
            break;
            case Actions::MeleeChase:
            {
                if (entity->GetComponent().GetCurrentAnimationID() == "Shadow_AxeIdle" &&
                    postSwingDelay <= 0)
                {
                    switch (rand() % 2)
                    {
                        case 0:
                            action = Actions::MeleeLungeHitCombo;
                            break;
                        case 1:
                            action = Actions::MeleeTurnHitCombo;
                            break;
                    }
                    swingCount = 1 + (currentDifficulty >= 3 ? 1 : 0) +
                                 (currentDifficulty >= 7 ? 1 : 0) +
                                 (currentDifficulty >= 14 ? 1 : 0);
                }
            }
            break;

            case Actions::MeleeLungeHitCombo:
            case Actions::MeleeTurnHitCombo:
            {
                double angleDeltaToUse = 10.0;
                if (action == Actions::MeleeTurnHitCombo)
                {
                    angleDeltaToUse = 40.0;
                }

                double distanceToUse = 200.0;
                if (action == Actions::MeleeLungeHitCombo)
                {
                    distanceToUse = 300.0;
                }

                distanceToUse *= baseForwardStrength / maxForwardStrength;

                if (swingCount > 0)
                {
                    if (entity->GetComponent().GetCurrentAnimationID() ==
                            "Shadow_AxeIdle" &&
                        distance < distanceToUse && angleDelta < angleDeltaToUse)
                    {
                        entity->GetComponent().SwitchAnimation("Shadow_AxeSwing");
                        swingCount--;
                    }
                }
                else
                {
                    action = Actions::MeleeChase;
                }
            }
            break;
        }

        if (entity->GetComponent().GetCurrentAnimationID() == "Shadow_PistolIdle" &&
            pistolShotCount > 0)
        {

            if (!playerOutOfPistolRange && counter >= nextShot)
            {
                nextShot = counter + Utility::ClampValue(35 - currentDifficulty, 3, 48);
                entity->GetComponent().SwitchAnimation("Shadow_PistolShoot");
                hitRecently = false;
                GenericWeaponFireLogic(15.0 + Globals::Difficulty(),
                                       4.0 - 0.25 * currentDifficulty);

                // 33% chance to not shoot immediately during next period, if player gets under cover
                // Overrides the hit result from the shot
                if (rand() % 3 == 0)
                {
                    hitRecently = false;
                }

                pistolShotCount--;
            }
        }

        if (entity->GetComponent().GetCurrentAnimationID() == "Shadow_AxeSwing")
        {
            SwingAxe();
        }

        lastFramePlayerPos = player->GetTransform().position;
    }

    void ShadowAI::GenericWeaponFireLogic(double damageToDeal, double angleDeltaToApply)
    {
        // Fires a non-piercing round that damages the player

        auto shotAngle = entity->GetTransform().direction;
        auto results   = Globals::Game()
                           .CreateRayCastHitList(entity->GetCollider().GetPosition(),
                                                 Vector2::NormalVector(
                                                     shotAngle + angleDeltaToApply) *
                                                         1800.0 +
                                                     entity->GetCollider().GetPosition());

        double tracerDrawDistance = 1800;
        if (results.size() > 1)
        {
            // Collide with first result that is an "enemy" or a static collider
            Collider* hit      = nullptr;
            double    distance = 0.0;
            for (auto& elem : results)
            {
                auto entity = elem.second->GetEntity();
                if (elem.second->GetCombatLayer() == Collider::CombatLayer::Players ||
                    elem.second->GetColliderType() == Collider::ColliderType::Static)
                {
                    hit      = elem.second;
                    distance = elem.first;
                    break;
                }
            }
            if (hit != nullptr)
            {
                auto entity = hit->GetEntity();
                if (dynamic_cast<Actor*>(entity) != nullptr)
                {
                    auto actor   = (Actor*)entity;
                    auto actorAI = actor->GetAI();
                    if (actorAI != nullptr)
                    {
                        this->OnAttackHit();
                        actorAI->OnHitByAttack(this->entity, damageToDeal);
                    }
                }
                tracerDrawDistance *= distance;
            }
        }
        Vector2 normal  = Vector2::NormalVector(shotAngle + angleDeltaToApply);
        Vector2 perpend = Vector2::NormalVector(normal.Angle() + 90.0);
        Globals::Game().CreateDefaultTracerEffect(entity->GetCollider().GetPosition() +
                                                      normal * 50 + perpend * 4,
                                                  normal * tracerDrawDistance +
                                                      entity->GetCollider().GetPosition() +
                                                      perpend * 4 + normal * 3.0);
    }

    void ShadowAI::OnDeath()
    {
        if (!EntityIsDeadAF())
        {
            if (!lastStand)
            {
                lastStand = true;
                action    = Actions::Recovery;
                entity->GetComponent().SwitchAnimation("Shadow_RecoveryStart");
                entity->GetComponent().SetDefaultAnimation("Shadow_Recovery");

                painCounter  = 540;
                auto& stats  = entity->GetStatsReference();
                stats.health = stats.maxHealth;
            }
            else
            {
                AI::OnDeath();
                entity->GetComponent().SwitchAnimation("Shadow_RecoveryStart");
                entity->GetComponent().SetDefaultAnimation("Shadow_Recovery");
                entity->GetCollider().QueueUnregisterFromGame();
                axe.QueueUnregisterFromGame();
                entity->GetComponent().SetLayer(GraphicsEngine::CommonLayers::OnFloor);
                destroyDelay = 9999;
            }
        }
    }

    void ShadowAI::OnAttackHit()
    {
        hitRecently = true;
    }

    void ShadowAI::OnHitByAttack(Actor* attacker, double damage)
    {
        if (!EntityIsDeadAF())
        {
            Globals::Audio().PlaySound("HurtBeta");

            if (action == Actions::Recovery)
            {
                damage *= 0.3;
            }
            else
            {
                painCounter += damage * 1.4;
                pistolCredits += damage;
                if (specialHurtSoundCooldown <= 0)
                {
                    Globals::Audio().PlaySound("ShadowHurt");
                    specialHurtSoundCooldown = 50 + rand() % 35;
                }
                else
                {
                    specialHurtSoundCooldown -= 5;
                    if (specialHurtSoundCooldown < 0)
                    {
                        specialHurtSoundCooldown = 0;
                    }
                }
            }

            AI::OnHitByAttack(attacker, damage);
        }
    }
} // namespace Game