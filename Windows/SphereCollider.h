#ifndef SPHERE_HEADER
#define SPHERE_HEADER

#include "PCHeader.h"
#include "Collider.h"

namespace Game {
	class SphereCollider : public Collider {
	protected:
		double radius;
	public:
		SphereCollider();
		SphereCollider(const Vector2& center, double radius, ColliderType type);

		void SetRadius(double radius);
		double GetRadius() const;

		virtual pair<Vector2, Vector2> GetBoundingBox();
	};
}

#endif
