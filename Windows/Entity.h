#ifndef ENTITY_HEADER
#define ENTITY_HEADER

#include "PCHeader.h"
#include "MiscUtility.h"
#include "SphereCollider.h"



namespace Game {
	class AI;

	enum EntityType {
		Unknown,
		Player,
		Fighter,
		FighterBulletProjectile,
		Knight,
		NonSentient // Spells, projectiles, etc.
	};

	class Entity {
	public:
	protected:
		Game::Transform transform; // Does not use center
		EntityType type;

		bool toBeDestroyed;
	public:
		Entity();
		Entity(EntityType type);
		virtual ~Entity();


		void SetType(EntityType type);
		EntityType GetType();

		void SetPosition(Vector2 position);
		Vector2& GetPosition(); // The reference returned can be modified / mounted to


		Transform& GetTransform();

		virtual void Update() = 0;

		inline void SignalDestruction() { toBeDestroyed = true; }
		inline bool IsDestructionSignalled() { return toBeDestroyed; }
	};
}

#endif