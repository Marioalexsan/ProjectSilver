#ifndef TEXTURE_HEADER
#define TEXTURE_HEADER

#include "STLHeader.h"
#include "MiscUtility.h"

namespace Game {

	// Base interface for drawable
	// Angle is from OY+ axis, clockwise

	class Drawable {
	protected:
		Point position;
		Point center;
		double rotation;

		int layer;
	public:
		Drawable();
		virtual void Draw() = 0;

		void SetAngle(double angle);
		void RotateBy(double angle);

		void MoveTo(Point position);
		void PushBy(Point amount);

		void SetCenter(Point center);

		void SetLayer(int layer);

		Point GetPosition();
		Point GetCenter();
		double GetRotation();
	};
}

#endif