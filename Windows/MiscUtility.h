#pragma once

#include "PCHeader.h"

namespace Game {
	namespace Math {
		const double PI = 3.14159265358979323846;

		inline double RadsToDeg(double rads) {
			return rads * (180.0 / PI);
		}

		inline double DegToRads(double degrees) {
			return degrees / (180.0 / PI);
		}

		double GetAngleDifference(double firstDirection, double secondDirection);
	};

	namespace Utility {
		SDL_Rect	MakeSDLRect(int x, int y, int w, int h);
		SDL_Point	MakeSDLPoint(int x, int y);

		int		ClampValue(int val, int min, int max);
		double	ClampValue(double val, double min, double max);

		//int    ScrollValue(int val, int leftBorder, int rightBorder);
		double	ScrollValue(double val, double leftBorder, double rightBorder);

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

		inline Vector2 operator+(const Vector2& other) const {
			return Vector2(x + other.x, y + other.y);
		}

		inline Vector2& operator+=(const Vector2& other) {
			x += other.x;
			y += other.y;
			return *this;
		}

		inline Vector2 operator-(const Vector2& other) const {
			return Vector2(x - other.x, y - other.y);
		}

		inline Vector2& operator-=(const Vector2& other) {
			x -= other.x;
			y -= other.y;
			return *this;
		}

		inline bool operator==(const Vector2& other) {
			return x == other.x && y == other.y;
		}

		inline bool operator!=(const Vector2& other) {
			return x != other.x || y != other.y;
		}

		inline Vector2 operator*(double scale) const {
			return Vector2(x * scale, y * scale);
		}

		inline Vector2& operator*=(double scale) {
			x *= scale;
			y *= scale;
			return *this;
		}

		inline Vector2 operator/(double scale) const {
			return Vector2(x / scale, y / scale);
		}

		inline Vector2& operator/=(double scale) {
			x /= scale;
			y /= scale;
			return *this;
		}

		static Vector2 NormalVector(double direction = 90.0);

		static const Vector2 Zero;
	};

	struct Transform {
		Vector2 position;
		Vector2 center;
		double direction;

		Transform();
		Transform(const Vector2& position, const Vector2& center, double direction);
	};

	// Reminder: I was dumb enough not to use the Rule of Three with this class. I deserve the crashes I got.
	// Null pointer checks in this class are redundant since it's not supposed to house a nullptr
	// Still, I'm just a mortal programmer, so they get to stay

	template<class T>
	class Trackable {
	private:
		T* object;
		bool ownsObject;
	public:
		Trackable():
			object(new T),
			ownsObject(true) {}

		Trackable(T* target):
			object(target),
			ownsObject(false) {
			if (target == nullptr) {
				// Nullptr not allowed
				object = new T;
				ownsObject = true;
				return;
			}
			object = target;
			ownsObject = false;
		}

		Trackable(const Trackable<T>& other):
			object(nullptr),
			ownsObject(true)
		{
			// This does NOT do Tracking. It creates its own copy of the value
			if (other.object != nullptr) {
				// Copy the other object
				object = new T(*other.object);
			}
			else {
				// Nullptr not allowed
				object = new T;
			}
		}

		Trackable<T>& operator=(const Trackable<T>& other) {
			// This does NOT do Tracking. It creates its own copy of the value
			if (ownsObject) {
				delete object;
			}
			if (other.object != nullptr) {
				// Copy the other object
				object = new T(*other.object);
			}
			else {
				// Nullptr not allowed
				object = new T;
			}
			ownsObject = true;

			return *this;
		}

		~Trackable() {
			if (ownsObject) {
				delete object;
			}
		}

		inline void Track(T* target) {
			// Does not accept nullptr.
			// Trackable<> should always point to an object. A dangling tracked pointer is, however, possible.
			if (target == nullptr) {
				return;
			}
			if (ownsObject) {
				delete object;
				ownsObject = false;
			}
			object = target;
		}

		inline void Untrack() {
			if (!ownsObject) {
				ownsObject = true;
				object = new T;
			}
		}

		inline bool IsTracking() {
			return !ownsObject;
		}

		inline T* operator->() {
			return object;
		}

		inline const T* operator->() const {
			return object;
		}

		inline T* Get() {
			return object;
		}
	};
}