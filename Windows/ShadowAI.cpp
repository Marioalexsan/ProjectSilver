#include "PCHeader.h"
#include "ShadowAI.h"
#include "Globals.h"
#include "Actor.h"

namespace Game {

    // ULTRA SUPER DUPER UNIMPLEMENTED
    // FOR ALL PURPOSES, THIS FILE IS BROKEN AND SHOULD NOT BE USED (until repaired)

    ShadowAI::ShadowAI():
        action(Recovery),
        axe(Game::Vector2(0, 0), 60, Game::Collider::ColliderType::Combat),
        AI(),
        lastFramePlayerPos(0.0, 0.0),
        recoveryPeriod(90)
    {
        axe.SetCombatDamage(35.0 + 5.0 * Globals::Difficulty());
        axe.SetCombatLayer(Collider::CombatLayer::None);
        axe.SetLayersToAttack({ Collider::CombatLayer::Players });
        axe.SetCollisionOptions({ Collider::CollisionOptions::DoNotHitRememberedEntities });
    }

    void ShadowAI::DecideAction() {
        const string& shadowAnimation = entity->GetComponent().GetCurrentAnimationID();
        auto& game = Globals::Game();
        auto player = Globals::ThePlayer();


        auto distance = (player->GetTransform().position - entity->GetTransform().position).Length();

        if (distance < 300.0) {
        }
        else {
        }
    }

    ShadowAI::~ShadowAI() {

    }

    void ShadowAI::Update() {
        if (!EntityIsDeadAF()) {
            return;
        }

        counter++;
        if (postSwingDelay > 0) {
            postSwingDelay--;
        }
        if (action == Actions::Recovery) {
            if (recoveryPeriod > 0) {
                recoveryPeriod--;
                if (recoveryPeriod == 0) {
                }
            }

        }
        

        const string& shadowAnimation = entity->GetComponent().GetCurrentAnimationID();
        auto& game = Globals::Game();
        auto player = Globals::ThePlayer();

        // Move Speed Logic
        double moveSpeed = 9.0;
        if (shadowAnimation == "Shadow_AxeSwing") {
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

        // Move Speed Direction

        double movePredictionStrength = 0.15;

        auto targetVector = player->GetTransform().position - this->entity->GetTransform().position + (player->GetTransform().position - lastFramePlayerPos) * movePredictionStrength * 60.0;

        auto targetDirection = targetVector.Angle();
        auto entityDirection = this->entity->GetTransform().direction;

        auto angleDelta = abs(targetDirection - entityDirection);
        angleDelta = angleDelta > 180.0 ? 360.0 - angleDelta : angleDelta;

        

        lastFramePlayerPos = player->GetTransform().position;
    }

    void ShadowAI::GenericWeaponFireLogic(double damageToDeal, double angleDeltaToApply) {
        // Fires a non-piercing round that damages the player

        auto mouse = Globals::Game().Input.GetMousePosition();
        auto shotAngle = this->entity->GetTransform().direction;
        auto results = Globals::Game().CreateRayCastHitList(entity->GetCollider().GetPosition(), Vector2::NormalVector(shotAngle + angleDeltaToApply) * 1800.0 + entity->GetCollider().GetPosition());
        if (results.size() > 1) {
            auto firstElem = results[0].second->GetEntity() == Globals::Game().GetThePlayer() ? results[0] : results[1];
            auto entity = firstElem.second->GetEntity();
            if (entity->GetType() == EntityType::Player && dynamic_cast<Actor*>(entity) != nullptr) {
                auto actor = (Actor*)entity;
                auto actorAI = actor->GetAI();
                if (actorAI != nullptr) {
                    actorAI->OnHitByAttack(this->entity, damageToDeal);
                }
            }
        }
    }

    void ShadowAI::OnDeath() {
        if (!EntityIsDeadAF()) {
            AI::OnDeath();
            Globals::Audio().PlaySound("Death");
            entity->GetComponent().SwitchAnimation("Player_Dead");
            entity->GetCollider().QueueUnregisterFromGame();
            entity->GetComponent().SetLayer(GraphicsEngine::CommonLayers::OnFloor);
            destroyDelay = 300;
        }
    }

    void ShadowAI::OnAttackHit() {

    }

    void ShadowAI::OnHitByAttack(Actor* attacker, double damage) {
        if (!EntityIsDeadAF()) {
            Globals::Audio().PlaySound("HurtBeta");
            AI::OnHitByAttack(attacker, damage);
        }
    }
}