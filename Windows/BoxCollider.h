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
		BoxCollider();
		BoxCollider(const Vector2& topLeft, double width, double height, ColliderType type);

		virtual const Vector2 GetPosition() const;
		const Vector2 GetCenteredPosition() const;

		void SetSize(double width, double height);
		pair<double, double> GetSize() const;

		void SetCenteredPosition(const Vector2& position);

		virtual pair<Vector2, Vector2> GetBoundingBox();
	};
}

#endif