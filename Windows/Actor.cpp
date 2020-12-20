#include "PCHeader.h"
#include "Actor.h"

namespace Game {
	void Actor::Update() {
		// Process knockback
		ApplyKnockbackStep();
	}

	void Actor::ApplyKnockbackStep() {
		position += knockback * 0.1;
		knockback *= 0.9;
		if (knockback.Length() < 0.5) {
			knockback = Vector2::Zero;
		}
	}

	void Actor::Move(Vector2 vector) {
		position += vector;
	}

	void Actor::MoveForward(double amount) {
		position += Vector2::NormalVector(direction) * amount;
	}

	Actor::ActorStats& Actor::GetStatsReference() {
		return baseStats;
	}
}