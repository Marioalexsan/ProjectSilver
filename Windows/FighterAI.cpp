#include "PCHeader.h"
#include "Globals.h"
#include "FighterAI.h"
#include "Actor.h"

namespace Game {

    FighterAI::FighterAI() :
        AI()
    {}

    void FighterAI::Update() {
        if (entity == nullptr) {
            return;
        }
        auto player = Globals::ThePlayer();
        auto targetVector = player->GetTransform().position - this->entity->GetTransform().position;
        auto targetDirection = targetVector.Angle();
        auto entityDirection = this->entity->GetTransform().direction;
        if (abs(targetDirection - entityDirection) > 180.0) {
            if (targetDirection > 180.0) {
                entity->GetTransform().direction -= 2.5;
            }
            else {
                entity->GetTransform().direction += 2.5;
            }
        }
        else {
            if (targetDirection - entityDirection > 0) {
                entity->GetTransform().direction += 2.5;
            }
            else {
                entity->GetTransform().direction -= 2.5;
            }
        }
        entity->GetTransform().direction = Utility::ScrollValue(entity->GetTransform().direction, 0.0, 360.0);
        entity->MoveForward(2.0);
    }

    void FighterAI::OnDeath() {

    }

    void FighterAI::OnAttackHit() {

    }

    void FighterAI::OnHitByAttack(Actor* attacker, double damage) {

    }
}