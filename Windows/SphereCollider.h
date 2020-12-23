#ifndef SPHERE_HEADER
#define SPHERE_HEADER

#include "PCHeader.h"
#include "Collider.h"

namespace Game {
	class SphereCollider : public Collider {
	protected:
		double radius;
	public:
		void SetRadius(double radius);
		double GetRadius() const;
	};
}

#endif
