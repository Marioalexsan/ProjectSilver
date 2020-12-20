#pragma once

#include "PCHeader.h"

namespace Game {
	namespace Math {
		const double PI = 3.14159265358979323846;
	};

	namespace Utility {
		SDL_Rect	MakeSDLRect(int x, int y, int w, int h);
		SDL_Point	MakeSDLPoint(int x, int y);

		int		ClampValue(int val, int min, int max);
		double	ClampValue(double val, double min, double max);

		bool	InRange(int val, int min, int max);
	}

	struct Vector2 {
		double x;
		double y;

		Vector2();
		Vector2(double, double);
		Vector2(int, int);

		double Length();
		double Angle();

		inline Vector2 operator+(const Vector2& other) {
			return Vector2(x + other.x, y + other.y);
		}

		inline Vector2& operator+=(const Vector2& other) {
			x += other.x;
			y += other.y;
			return *this;
		}

		inline Vector2 operator-(const Vector2& other) {
			return Vector2(x - other.x, y - other.y);
		}

		inline Vector2& operator-=(const Vector2& other) {
			x -= other.x;
			y -= other.y;
			return *this;
		}

		inline Vector2 operator*(double scale) {
			return Vector2(x * scale, y * scale);
		}

		inline Vector2& operator*=(double scale) {
			x *= scale;
			y *= scale;
			return *this;
		}

		static Vector2 NormalVector(double direction = 90.0);

		static const Vector2 Zero;
	};
}