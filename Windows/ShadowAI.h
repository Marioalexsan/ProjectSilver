#ifndef SHADOWAI_HEADER
#define SHADOWAI_HEADER

#include "PCHeader.h"
#include "AI.h"
#include "BasicText.h"
#include "SphereCollider.h"
#include "Sprite.h"
#include "ShadowHPBar.h"

namespace Game {
	class ShadowAI : public AI {
		enum class Actions {
			DoingAbsolutelyNothing,
			Recovery,
			MeleeChase,
			PistolRetaliation,
			MeleeLungeHitCombo,
			MeleeTurnHitCombo,
		};
		
		bool lastStand;
		bool seesPlayer;

		bool strafesLeft;
		int nextStrafeChange;

		bool doingSwing;
		int postSwingDelay;
		int swingCount;
		
		double pistolCredits;
		int pistolShotCount;
		int pistolUseCooldown;
		int nextShot;
		bool hitRecently;

		double hurtEvasionCredits;
		int hurtEvasionTriggerDelay;
		int hurtEvasionCooldown;

		double painCounter;
		double currentSpeed;

		int specialHurtSoundCooldown;

		Actions action;

		ShadowHPBar hp;

		Game::SphereCollider axe;

		Vector2 lastFramePlayerPos;

		void DecideAction();
		void CounterLogic();
		void SwingAxe();
		double CalculateSwingFactor();
		bool HasLineOfSight();


		void GenericWeaponFireLogic(double damageToDeal, double angleDeltaToApply = 0.0);
	public:
		ShadowAI();
		~ShadowAI();

		int GetDifficultyLevel();

		virtual void OnAttackHit();
		virtual void OnHitByAttack(Actor* attacker, double damage);

		virtual void Update();

		inline bool InLastStand() { return lastStand; }

		virtual void OnDeath();
	};
}

#endif