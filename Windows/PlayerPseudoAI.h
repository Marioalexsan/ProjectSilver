#ifndef PLAYERPSEUDOAI_HEADER
#define PLAYERPSEUDOAI_HEADER

#include "PCHeader.h"
#include "AI.h"
#include "BasicText.h"

namespace Game {
	class PlayerPseudoAI : public AI {
		int currentPistolAmmo;
		int maxPistolAmmo;
		bool wasReloadingPistol;
		bool hasShield;
		bool inShield;

		int regenCounter;
		int shieldRegenCounter;
		int shieldFadeOutDelay;

		int perfectGuardCounter;

		Game::BasicText playerHealth;
		Game::BasicText shieldHealth;
		Game::BasicText gunAmmo;
	public:
		PlayerPseudoAI();
		~PlayerPseudoAI();

		virtual void OnAttackHit();
		virtual void OnHitByAttack(Actor* attacker, double damage);

		virtual void Update();

		virtual void OnDeath();
	};
}

#endif