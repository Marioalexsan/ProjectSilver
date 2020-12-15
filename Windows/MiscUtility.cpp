#include "MiscUtility.h"
#include <cmath>

namespace Game {

	Point::Point():
		x(0),
		y(0) {}

	Point::Point(double x, double y):
		x(x),
		y(y) {}

	double Point::Length() {
		return sqrt(x * x + y * y);
	}

	// OY+ -> Vector in clockwise direction = angle in degrees

	double Point::Angle() {
		return 360.0 - atan2(y, x) * 180 / Math::PI + (y > 0 && x > 0) ? 270.0 : -90.0;
	}


	SDL_Rect Utility::MakeSDLRect(int x, int y, int w, int h) {
		SDL_Rect lmao;
		lmao.x = x;
		lmao.y = y;
		lmao.w = w;
		lmao.h = h;
		return lmao;
	}

	SDL_Point Utility::MakeSDLPoint(int x, int y) {
		SDL_Point lmao;
		lmao.x = x;
		lmao.y = y;
		return lmao;
	}
}