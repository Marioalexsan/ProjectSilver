#ifndef TEXTURE_HEADER
#define TEXTURE_HEADER

#include "PCHeader.h"
#include "MiscUtility.h"

namespace Game {

	// Base interface for drawable
	// Angle is from OY+ axis, clockwise

	class Drawable {
	protected:
		Game::Transform transform;

		int layer;
		bool relativeToCamera;

		uint64_t drawableGraphicsID = 0;
	public:
		Drawable();
		virtual ~Drawable();
		virtual void Draw() = 0;

		const Game::Transform& GetTransform();

		void SetDirection(double direction);
		void Rotate(double angle);

		void SetPosition(const Vector2& position);
		void Move(const Vector2& amount);

		void SetCenter(const Vector2& center);

		int  GetLayer();
		void SetLayer(int layer);

		void SetRelativeToCamera(bool relativity);

		void RegisterDrawable();
		void UnregisterDrawable();
	};
}

#endif