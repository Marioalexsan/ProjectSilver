#include "PCHeader.h"

#include "MiscUtility.h"

namespace Game {

	Vector2::Vector2():
		x(0),
		y(0) {}

	Vector2::Vector2(double x, double y):
		x(x),
		y(y) {}

	Vector2::Vector2(int x, int y) :
		x(double(x)),
		y(double(y)) {}

	double Vector2::Length() {
		return sqrt(x * x + y * y);
	}

	//  Calculates angle from OY- to Vector, 
	double Vector2::Angle() {
		return atan2(y, x) * 180 / Math::PI + ((y < 0 && x < 0) ? 450.0 : 90.0);
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

	int Utility::ClampValue(int val, int min, int max) {
		return val < min ? min : (val >= max ? max : val);
	}

	double Utility::ClampValue(double val, double min, double max) {
		return val < min ? min : (val >= max ? max : val);
	}
}