#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/AI.hpp>
#include <ProjectSilver/Entity.hpp>

namespace Game {
	Entity::Entity():
		type(EntityType::Unknown),
		transform(),
		toBeDestroyed(false) {}

	Entity::Entity(EntityType type):
		type(type),
		transform(),
		toBeDestroyed(false) {}

	Entity::~Entity() {
	}

	void Entity::SetType(EntityType type) {
		this->type = type;
	}

	EntityType Entity::GetType() {
		return type;
	}

	void Entity::SetPosition(Vector2 position) {
		transform.position = position;
	}

	Vector2& Entity::GetPosition() {
		return transform.position;
	}

	Transform& Entity::GetTransform() {
		return transform;
	}


}