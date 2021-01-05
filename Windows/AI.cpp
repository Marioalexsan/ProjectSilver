#include "PCHeader.h"
#include "AI.h"
#include "Actor.h"

namespace Game {
	AI::AI(Actor* owner):
		counter(0),
		entity(owner) {}

	AI::AI():
		counter(0),
		entity(nullptr) {}

	AI::~AI() {
	}

	bool AI::EntityIsDeadAF() {
		return entity == nullptr || entity->GetStatsReference().isDead;
	}

	void AI::OnAttackHit() {}

	void AI::OnHitByAttack(Actor* attacker, double damage) {
		if (entity != nullptr) {
			auto& stats = entity->GetStatsReference();
			if (damage < 0.0) {
				return;
			}
			if (stats.currentInvincibilityFrames > 0) {
				return;
			}
			stats.currentInvincibilityFrames = stats.onHitInvincibilityFrames;
			stats.health -= damage;
			if (stats.health <= 0.0) {
				OnDeath();
			}
		}
	}

	void AI::OnDeath() {
		if (entity != nullptr) {
			auto stats = entity->GetStatsReference().isDead = true;
		}
	}
}