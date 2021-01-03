#ifndef PLAYERPSEUDOAI_HEADER
#define PLAYERPSEUDOAI_HEADER

#include "PCHeader.h"
#include "AI.h"

namespace Game {
	class PlayerPseudoAI : public AI {
	public:
		PlayerPseudoAI();

		virtual void OnAttackHit();
		virtual void OnHitByAttack(Actor* attacker, double damage);

		virtual void Update();

		virtual void OnDeath();
	};
}

#endif