#ifndef FIGHTERAI_HEADER
#define FIGHTERAI_HEADER

#include "PCHeader.h"
#include "AI.h"

namespace Game {
	class FighterAI : public AI {
	public:
		FighterAI();

		virtual void OnAttackHit();
		virtual void OnHitByAttack(Actor* attacker, double damage);

		virtual void Update();

		virtual void OnDeath();
	};
}

#endif