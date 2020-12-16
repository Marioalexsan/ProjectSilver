#include "PCHeader.h"

#include "Drawable.h"
#include "GraphicsEngine.h"

namespace Game {
	Drawable::Drawable() :
		position({ 0, 0 }),
		center({ 0, 0 }),
		rotation(0.0),
		layer(Game::GraphicsEngine::WorldBaseLayer) {}

	void Drawable::SetAngle(double angle) {
		this->rotation = angle;
	}

	void Drawable::RotateBy(double angle) {
		this->rotation += angle;
	}

	void Drawable::MoveTo(Point position) {
		this->position = position;
	}

	void Drawable::PushBy(Point amount) {
		this->position += amount;
	}

	void Drawable::SetCenter(Point center) {
		this->center = center;
	}

	void Drawable::SetLayer(int layer) {
		this->layer = layer;
	}

	Point Drawable::GetPosition() {
		return position;
	}
	Point Drawable::GetCenter() {
		return center;
	}
	double Drawable::GetRotation() {
		return rotation;
	}
}