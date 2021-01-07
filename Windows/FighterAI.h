#ifndef FIGHTERAI_HEADER
#define FIGHTERAI_HEADER

#include "PCHeader.h"
#include "AI.h"

namespace Game {
	class FighterAI : public AI {
	private:
		bool strafesLeft;
		int nextStrafeChange;
		int nextShot;
		int previousShot;

		double predictionStrengthToUse;

		Vector2 lastFramePlayerPos;
	public:
		FighterAI();

		virtual void OnAttackHit();
		virtual void OnHitByAttack(Actor* attacker, double damage);

		virtual void Update();

		virtual void OnDeath();
	};
}

#endif