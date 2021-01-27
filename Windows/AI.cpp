#include "PCHeader.h"
#include "AI.h"
#include "Actor.h"

namespace Game {
	AI::AI(Actor* owner):
		counter(-45),
		entity(owner) {}

	AI::AI():
		counter(-45),
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
			entity->GetStatsReference().isDead = true;
		}
	}

	bool AI::DelayedSpawningLogic() {
		// This acts as delayed spawning
		if (counter == -45) {
			entity->GetCollider().UnregisterFromGame();
		}

		if (counter == 0) {
			entity->GetCollider().RegisterToGame();
		}

		if (counter <= 0) {
			entity->GetComponent().SetAlpha(uint8_t((45 + counter) * 255 / 45));
			return true;
		}
		return false;
	}

	bool AI::ProcessGenericDestroyDelay() {
		if (entity->GetStatsReference().isDead == true) {
			destroyDelay--;
			if (destroyDelay <= 150) {
				entity->GetComponent().SetAlpha(uint8_t(destroyDelay / 150.0 * 255.0));
			}
			if (destroyDelay == 0) {
				entity->SignalDestruction();
			}
			return true;
		}
		return false;
	}
}