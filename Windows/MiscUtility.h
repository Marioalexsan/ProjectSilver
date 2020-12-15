#pragma once

#include <SDL.h>

namespace Game {
	namespace Math {
		const double PI = 3.14159265358979323846;
	};

	namespace Utility {
		SDL_Rect MakeSDLRect(int x, int y, int w, int h);
		SDL_Point MakeSDLPoint(int x, int y);
	}

	struct Point {
		double x;
		double y;

		Point();
		Point(double, double);

		double Length();
		double Angle();

		inline Point operator+(const Point& other) {
			return Point(x + other.x, y + other.y);
		}

		inline void operator+=(const Point& other) {
			x += other.x;
			y += other.y;
		}

		inline Point operator-(const Point& other) {
			return Point(x - other.x, y - other.y);
		}

		inline void operator-=(const Point& other) {
			x -= other.x;
			y -= other.y;
		}

		
	};
}