#ifndef AI_HEADER
#define AI_HEADER

#include "PCHeader.h"

namespace Game {
	class Actor;
	class Entity;

	class AI {
	protected:
		int counter;
		int disableUpdatesCounter;

		Actor* entity;

	public:
		AI(Actor* owner);
		AI();

		~AI();

		inline void SetEntity(Actor* entity) {
			this->entity = entity;
		}

		inline Actor* GetEntity() {
			return entity;
		}

		virtual void OnAttackHit() = 0;
		virtual void OnHitByAttack(Actor* attacker, double damage) = 0;
		virtual void OnDeath();
		virtual void Update() = 0;
	};
}

#endif