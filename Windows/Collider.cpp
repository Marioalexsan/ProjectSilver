#include "PCHeader.h"
#include "Collider.h"
#include "Globals.h"
#include "Entity.h"

namespace Game {
	Collider::Collider():
		destructionSignalled(false),
		alsoSignalEntityDestruction(false),
		transform(),
		mass(80.0),
		owner(nullptr),
		type(Static),
		damage(0.0),
		combatLayer(None) {}

	Collider::Collider(const Vector2& position, ColliderType type):
		destructionSignalled(false),
		alsoSignalEntityDestruction(false),
		transform(),
		mass(80.0),
		owner(nullptr),
		type(type),
		damage(0.0),
		combatLayer(None)
	{
		transform->position = position;
	}

	Collider::~Collider() {
		// Make sure we don't have dangling pointers
		UnregisterFromGame();
	}

	Trackable<Transform>& Collider::GetTransform() {
		return transform;
	}

	const Game::Vector2 Collider::GetPosition() const {
		return transform->position;
	}

	void Collider::SetPosition(const Vector2& position) {
		transform->position = position;
	}

	void Collider::Move(const Vector2& amount) {
		transform->position += amount;
	}

	void Collider::SignalDestruction(){
		if (info.find(Collider::CollisionOptions::DestroyCombatColliderAgainstStatic) != info.end()) {
			destructionSignalled = true;
			if (alsoSignalEntityDestruction && owner != nullptr) {
				owner->SignalDestruction();
			}
		}
	}

	void Collider::RegisterToGame() {
		Globals::Game().AddCollider(this);
	}

	void Collider::UnregisterFromGame() {
		Globals::Game().RemoveCollider(this);
	}

	void Collider::QueueUnregisterFromGame() {
		Globals::Game().AddColliderToRemovalQueue(this);
	}
}