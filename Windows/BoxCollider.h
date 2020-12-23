#ifndef BOX_HEADER
#define BOX_HEADER

#include "PCHeader.h"
#include "Collider.h"
#include "SphereCollider.h"

namespace Game {
	class BoxCollider : public Collider {
	protected:
		double width;
		double height;

		// Position indicates top left corner
	public:
		void SetSize(double width, double height);
		pair<double, double> GetSize() const;

	};
}

#endif