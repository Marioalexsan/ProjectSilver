#include "PCHeader.h"
#include "BoxCollider.h"


namespace Game {
	void BoxCollider::SetSize(double width, double height) {
		this->width = width;
		this->height = height;
	}

	pair<double, double> BoxCollider::GetSize() const {
		return { width, height };
	}
}