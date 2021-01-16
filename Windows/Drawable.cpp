#include "PCHeader.h"

#include "Drawable.h"
#include "GraphicsEngine.h"
#include "Globals.h"

namespace Game {
	const Color Color::White = { 255, 255, 255 };
	const Color Color::Black = { 0, 0, 0 };
	const Color Color::Red = { 255, 0, 0 };
	const Color Color::Green = { 0, 128, 0 }; // A dark green, in a way
	const Color Color::Blue = { 0, 0, 255 };
	const Color Color::Yellow = { 255, 255, 0 };
	const Color Color::Orange = { 255, 165, 0 };
	const Color Color::Purple = { 128, 0, 128 };
	const Color Color::Aqua = { 0, 255, 255 };
	const Color Color::Gray = { 128, 128, 128 };


	Drawable::Drawable() :
		color(Color::White),
		alpha(255),
		transform(),
		relativeToCamera(true),
		layer(Game::GraphicsEngine::CommonLayers::DefaultHeight),
		drawableGraphicsID(0) {}

	Drawable::~Drawable() {
		// Destroy existing pointers to this drawable
		UnregisterFromGame();
	}

	Trackable<Game::Transform>& Drawable::GetTransform() {
		return transform;
	}

	void Drawable::SetDirection(double direction) {
		transform->direction = direction;
	}

	void Drawable::Rotate(double angle) {
		transform->direction += angle;
	}

	void Drawable::SetPosition(const Vector2& position) {
		transform->position = position;
	}

	void Drawable::Move(const Vector2& amount) {
		transform->position += amount;
	}

	void Drawable::SetCenter(const Vector2& center) {
		transform->center = center;
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


	void Drawable::RegisterToGame() {
		if (drawableGraphicsID == 0) {
			drawableGraphicsID = Globals::Graphics().AddDrawable(this);
		}
	}

	void Drawable::UnregisterFromGame() {
		Globals::Graphics().RemoveDrawable(drawableGraphicsID);
		drawableGraphicsID = 0;
	}
}