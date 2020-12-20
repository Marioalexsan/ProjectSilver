#ifndef ENTITY_HEADER
#define ENTITY_HEADER

#include "PCHeader.h"
#include "MiscUtility.h"

namespace Game {
	class Entity {
	public:
		enum EntityType {
			Unknown,
			WorldActor,
			StaticEnvironment
		};
	protected:
		// Are sens oare de facut getter / setter daca nu fac nimic in ele?!

		Vector2 position;
		double direction;
		EntityType type;
	public:
		Entity();
		Entity(EntityType type);

		void SetType(EntityType type);
		EntityType GetType();

		void SetPosition(Vector2 position);
		Vector2 GetPosition(Vector2);

		virtual void Update() = 0;
	};
}

#endif