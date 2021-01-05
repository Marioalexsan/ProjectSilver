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

		Game::BasicText playerHealth;
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