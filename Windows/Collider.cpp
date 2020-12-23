#include "PCHeader.h"
#include "Collider.h"

namespace Game {
	Collider::Collider():
		position(),
		mass(100.0),
		ownerType(Environment),
		colType(Static),
		combatType(Generic) {}

	Collider::Collider(const Vector2& position, double mass):
		position(position),
		mass(mass),
		ownerType(Environment),
		colType(Static),
		combatType(Generic) {}

	const Game::Vector2& Collider::GetPosition() const {
		return position;
	}

	void Collider::SetPosition(const Vector2& position) {
		this->position = position;
	}

	void Collider::Move(const Vector2& amount) {
		this->position += amount;
	}

	void Collider::SetSelectivity(ColliderOwners owner, ColliderTypes type, ColliderCombatRole role) {
		info.ownerType = owner;
		info.colType = type;
		info.combatType = role;
	}

	Collider::ColliderInfo Collider::GetSelectivity() {
		return info;
	}


}