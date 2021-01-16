#ifndef PLAYERPSEUDOAI_HEADER
#define PLAYERPSEUDOAI_HEADER

#include "PCHeader.h"
#include "AI.h"
#include "BasicText.h"
#include "SphereCollider.h"
#include "Sprite.h"

namespace Game {
	class ShadowAI : public AI {
		enum Actions {
			Recovery,
			UsePistol,
			UseMelee
		};
		int postSwingDelay;
		int recoveryPeriod;

		Actions action;

		Game::SphereCollider axe;

		Vector2 lastFramePlayerPos;

		void DecideAction();

		void GenericWeaponFireLogic(double damageToDeal, double angleDeltaToApply = 0.0);
	public:
		ShadowAI();
		~ShadowAI();

		virtual void OnAttackHit();
		virtual void OnHitByAttack(Actor* attacker, double damage);

		virtual void Update();

		virtual void OnDeath();
	};
}

#endif