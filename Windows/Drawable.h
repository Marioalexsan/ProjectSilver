#ifndef TEXTURE_HEADER
#define TEXTURE_HEADER

#include "PCHeader.h"

namespace Game {

	// Base interface for drawable
	// Angle is from OY+ axis, clockwise

	class Drawable {
	protected:
		Trackable<Game::Transform> transform;

		int layer;
		bool relativeToCamera;

		uint8_t alpha;

		uint64_t drawableGraphicsID = 0;
	public:
		Drawable();
		virtual ~Drawable();
		virtual void Draw() = 0;

		Trackable<Game::Transform>& GetTransform();

		void SetDirection(double direction);
		void Rotate(double angle);

		inline void SetAlpha(uint8_t alpha) { this->alpha = alpha; }

		inline uint8_t GetAlpha() { return alpha; }

		void SetPosition(const Vector2& position);
		void Move(const Vector2& amount);

		void SetCenter(const Vector2& center);

		int  GetLayer();
		void SetLayer(int layer);

		void SetRelativeToCamera(bool relativity);

		void RegisterToGame();
		void UnregisterFromGame();
	};
}

#endif