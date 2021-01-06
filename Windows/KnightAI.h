#ifndef KNIGHTAI_HEADER
#define KNIGHTAI_HEADER

#include "PCHeader.h"
#include "AI.h"

namespace Game {
	class KnightAI : public AI {
	private:
		bool strafesLeft;
		int nextStrafeChange;
		bool doingSwing;

		int postSwingDelay;

		SphereCollider sword;

		Vector2 lastFramePlayerPos;

	public:
		KnightAI();

		virtual void OnAttackHit();
		virtual void OnHitByAttack(Actor* attacker, double damage);

		virtual void Update();

		virtual void OnDeath();
	};
}

#endif