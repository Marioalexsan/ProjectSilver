#ifndef CHASERAI_HEADER
#define CHASERAI_HEADER

#include "PCHeader.h"
#include "SphereCollider.h"
#include "AI.h"

namespace Game {
	class ChaserAI : public AI {
	private:
		bool strafesLeft;
		int nextStrafeChange;
		bool doingSwing;

		int postSwingDelay;
		int painCounter;

		double currentSpeed;
		int fastSwingCounter;

		SphereCollider axe;

		Vector2 lastFramePlayerPos;

		int specialHurtSoundCooldown;

	public:
		ChaserAI();

		virtual void OnAttackHit();
		virtual void OnHitByAttack(Actor* attacker, double damage);

		virtual void Update();

		virtual void OnDeath();
	};
}

#endif