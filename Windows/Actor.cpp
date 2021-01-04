#include "PCHeader.h"
#include "Actor.h"
#include "Globals.h"
#include "AI.h"

namespace Game {
	Actor::Actor(AI* brain) :
		baseStats(),
		Entity()
	{
		this->brain = brain;
		if (brain != nullptr) {
			brain->SetEntity(this);
		}
		// Actor will own brain from this point onwards

		// Collider and RenderComponent refer to, and update, the entity position.
		render.GetTransform().Track(&this->transform);
		collider.GetTransform().Track(&this->transform);

		collider.SetColliderType(Collider::ColliderType::CombatDynamic);
		collider.SetRadius(50);
		collider.SetOwner(this);

		collider.RegisterToGame();
		render.RegisterToGame();
	}

	Actor::~Actor() {
		// Cleanup
		delete brain;
		collider.UnregisterFromGame();
		render.UnregisterFromGame();
	}

	void Actor::Update() {
		// Process knockback
		baseStats.currentInvincibilityFrames--;
		ApplyKnockbackStep();
		ApplyKnockback();
		
		brain->Update();
		render.Update();
	}

	void Actor::ApplyKnockbackStep() {
		transform.position += knockback * 0.1;
		knockback *= 0.9;
		if (knockback.Length() < 0.5) {
			knockback = Vector2::Zero;
		}
	}

	void Actor::ApplyKnockback() {
		Move(knockback * Utility::ClampValue(baseStats.knockbackResistance, 0.0, 100.0) * (1.0 / 60.0));
	}

	void Actor::Move(Vector2 vector) {
		transform.position += vector;
	}

	void Actor::MoveForward(double amount) {
		transform.position += Vector2::NormalVector(transform.direction) * amount;
	}

	Actor::ActorStats& Actor::GetStatsReference() {
		return baseStats;
	}
}