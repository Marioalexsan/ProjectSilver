#include "PCHeader.h"

#include "Drawable.h"
#include "GraphicsEngine.h"
#include "Globals.h"

namespace Game {
	Drawable::Drawable() :
		transform(),
		relativeToCamera(true),
		layer(Game::GraphicsEngine::CommonLayers::WorldBase),
		drawableGraphicsID(0) {}

	Drawable::~Drawable() {
		// Destroy existing pointers to this drawable
		UnregisterDrawable();
	}

	const Game::Transform& Drawable::GetTransform() {
		return transform;
	}

	void Drawable::SetDirection(double direction) {
		transform.direction = direction;
	}

	void Drawable::Rotate(double angle) {
		transform.direction += angle;
	}

	void Drawable::SetPosition(const Vector2& position) {
		transform.position = position;
	}

	void Drawable::Move(const Vector2& amount) {
		transform.position += amount;
	}

	void Drawable::SetCenter(const Vector2& center) {
		transform.center = center;
	}

	int Drawable::GetLayer() {
		return layer;
	}

	void Drawable::SetLayer(int layer) {
		this->layer = layer;
	}

	void Drawable::SetRelativeToCamera(bool relativity) {
		relativeToCamera = relativity;
	}


	void Drawable::RegisterDrawable() {
		if (drawableGraphicsID == 0) {
			drawableGraphicsID = Globals::Graphics().AddDrawable(this);
		}
	}

	void Drawable::UnregisterDrawable() {
		Globals::Graphics().RemoveDrawable(drawableGraphicsID);
		drawableGraphicsID = 0;
	}
}