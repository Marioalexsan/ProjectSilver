#include "PCHeader.h"

#include "Drawable.h"
#include "GraphicsEngine.h"

namespace Game {
	Drawable::Drawable() :
		position({ 0, 0 }),
		center({ 0, 0 }),
		rotation(0.0),
		relativeToCamera(true),
		layer(Game::GraphicsEngine::CommonLayers::WorldBase) {}

	void Drawable::SetAngle(double angle) {
		this->rotation = angle;
	}

	void Drawable::RotateBy(double angle) {
		this->rotation += angle;
	}

	void Drawable::MoveTo(Vector2 position) {
		this->position = position;
	}

	void Drawable::PushBy(Vector2 amount) {
		this->position += amount;
	}

	void Drawable::SetCenter(Vector2 center) {
		this->center = center;
	}

	int Drawable::GetLayer() {
		return layer;
	}

	void Drawable::SetLayer(int layer) {
		this->layer = layer;
	}

	Vector2 Drawable::GetPosition() {
		return position;
	}
	Vector2 Drawable::GetCenter() {
		return center;
	}
	double Drawable::GetRotation() {
		return rotation;
	}

	void Drawable::SetRelativeToCamera(bool relativity) {
		relativeToCamera = relativity;
	}
}