module;

#include <ProjectSilver/PCHeader.hpp>

module ProjectSilver.AI;
import ProjectSilver.Actor;
import ProjectSilver.Globals;
import ProjectSilver.MiscUtility;

namespace Game {
	AI::AI(Actor* owner):
		smoothSpeed(0.0, 0.0),
		counter(-45),
		entity(owner) {}

	AI::AI():
		smoothSpeed(0.0, 0.0),
		counter(-45),
		entity(nullptr) {}

	AI::~AI() {
	}

	bool AI::EntityIsDeadAF() {
		return entity == nullptr || entity->GetStatsReference().isDead;
	}

	bool AI::ProcessGenericDestroyDelay() {
		if (entity->GetStatsReference().isDead == true) {
			destroyDelay--;
			if (destroyDelay <= 150) {
				entity->GetComponent().SetAlpha(uint8_t(destroyDelay * 255 / 150));
			}
			if (destroyDelay == 0) {
				entity->SignalDestruction();
			}
			return true;
		}
		return false;
	}

	bool AI::DelayedSpawningLogic() {
		// This acts as delayed spawning
		SphereCollider& collider = entity->GetCollider();
		if (counter < 0 && collider.IsRegistered()) {
			collider.UnregisterFromGame();
		}

		if (counter == 0) {
			entity->GetCollider().RegisterToGame();
		}

		if (counter <= 0) {
			entity->GetComponent().SetAlpha(uint8_t(Utility::ClampValue(45 + counter, 0, 45) * 255 / 45));
			return true;
		}
		return false;
	}

	bool AI::PlayerIsUnavailable() {
		auto player = Globals::ThePlayer();
		return player == nullptr || ((Actor*)player)->GetStatsReference().isDead;
	}

	bool AI::HasPlayerLineOfSight() {
		if (PlayerIsUnavailable()) {
			return false;
		}

		auto results = Globals::Game().CreateRayCastHitList(entity->GetCollider().GetPosition(), ((Actor*)Globals::ThePlayer())->GetCollider().GetPosition());

		if (results.size() > 1) {
			// Collide with first result that is a static collider
			Collider* hit = nullptr;
			double distance = 0.0;
			for (auto& elem : results) {
				auto entity = elem.second->GetEntity();
				if (elem.second->GetColliderType() == Collider::ColliderType::Static) {
					hit = elem.second;
					distance = elem.first;
					break;
				}
			}
			if (hit != nullptr) {
				return false;
			}
			return true;
		}
		return false;
	}

	bool AI::HasLineOfSight(Vector2 point) {
		if (PlayerIsUnavailable()) {
			return false;
		}

		auto results = Globals::Game().CreateRayCastHitList(entity->GetCollider().GetPosition(), point);

		// Collide with first result that is a static collider
		Collider* hit = nullptr;
		double distance = 0.0;
		for (auto& elem : results) {
			auto entity = elem.second->GetEntity();
			if (elem.second->GetColliderType() == Collider::ColliderType::Static) {
				hit = elem.second;
				distance = elem.first;
				break;
			}
		}
		if (hit != nullptr) {
			return false;
		}
		return true;
	}

	void AI::OnAttackHit() {}

	void AI::OnHitByAttack(Actor* attacker, double damage) {
		if (entity != nullptr) {
			auto& stats = entity->GetStatsReference();
			if (stats.invulnerable || stats.currentInvincibilityFrames > 0 || damage < 0.0) {
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

	
}