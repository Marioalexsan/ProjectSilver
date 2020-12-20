#ifndef TEXTURE_HEADER
#define TEXTURE_HEADER

#include "PCHeader.h"
#include "MiscUtility.h"

namespace Game {

	// Base interface for drawable
	// Angle is from OY+ axis, clockwise

	class Drawable {
	protected:
		Vector2 position;
		Vector2 center;
		double rotation;

		int layer;

		bool relativeToCamera;

		uint64_t drawableGraphicsID = 0;
	public:
		Drawable();
		virtual ~Drawable();
		virtual void Draw() = 0;

		void SetAngle(double angle);
		void RotateBy(double angle);

		void MoveTo(Vector2 position);
		void PushBy(Vector2 amount);

		void SetCenter(Vector2 center);

		void SetLayer(int layer);
		int  GetLayer();

		Vector2 GetPosition();
		Vector2 GetCenter();
		double GetRotation();

		void SetRelativeToCamera(bool relativity);

		void AddDrawableToGraphics();
		void RemoveDrawableFromGraphics();
	};
}

#endif