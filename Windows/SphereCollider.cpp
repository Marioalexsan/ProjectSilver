#include "PCHeader.h"
#include "SphereCollider.h"

namespace Game {
	void SphereCollider::SetRadius(double radius) {
		this->radius = radius;
	}

	double SphereCollider::GetRadius() const {
		return radius;
	}
}