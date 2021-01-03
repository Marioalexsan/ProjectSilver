#include "PCHeader.h"
#include "BoxCollider.h"


namespace Game {
	BoxCollider::BoxCollider():
		Collider(Vector2::Zero, Static),
		width(10.0),
		height(10.0) {}

	BoxCollider::BoxCollider(const Vector2& topLeft, double width, double height, ColliderType type):
		Collider(topLeft, type),
		width(abs(width)),
		height(abs(height)) {}

	void BoxCollider::SetSize(double width, double height) {
		this->width = abs(width);
		this->height = abs(height);
	}

	pair<double, double> BoxCollider::GetSize() const {
		return { width, height };
	}

	const Vector2 BoxCollider::GetPosition() const {
		return transform->position;
	}

	const Vector2 BoxCollider::GetCenteredPosition() const {
		return GetPosition() + Vector2(width / 2.0, height / 2.0);
	}

	void BoxCollider::SetCenteredPosition(const Vector2& position) {
		SetPosition(position - Vector2(width / 2.0, height / 2.0));
	}

	pair<Vector2, Vector2> BoxCollider::GetBoundingBox() {
		auto pos = GetPosition();
		return { Vector2(pos.x, pos.y), Vector2(pos.x + width, pos.y + height) };
	}
}