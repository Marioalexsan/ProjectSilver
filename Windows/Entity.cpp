#include "PCHeader.h"
#include "Entity.h"

namespace Game {
	Entity::Entity():
		type(Unknown),
		position(Vector2::Zero),
		direction(0.0) {}

	Entity::Entity(EntityType type):
		type(type),
		position(Vector2::Zero),
		direction(0.0) {}

	void Entity::SetType(EntityType type) {
		this->type = type;
	}

	Entity::EntityType Entity::GetType() {
		return type;
	}

	void Entity::SetPosition(Vector2 position) {
		this->position = position;
	}

	Vector2 Entity::GetPosition(Vector2) {
		return position;
	}


}