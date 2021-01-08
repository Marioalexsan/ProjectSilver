#ifndef PLAYERPSEUDOAI_HEADER
#define PLAYERPSEUDOAI_HEADER

#include "PCHeader.h"
#include "AI.h"
#include "BasicText.h"
#include "SphereCollider.h"
#include "Sprite.h"

namespace Game {
	class PlayerPseudoAI : public AI {
		int currentPistolAmmo;
		int maxPistolAmmo;
		bool wasReloadingPistol;

		int currentRifleAmmo;
		int maxRifleAmmo;
		int rifleAmmoPool;
		double rifleRecoil;
		bool wasReloadingRifle;

		bool hasShield;
		bool inShield;
		bool doingSwing;

		bool wasInWeaponSwitch;

		int regenCounter;
		int shieldRegenCounter;
		int shieldFadeOutDelay;

		int staminaRegenCounter;
		int staminaFadeOutDelay;

		int perfectGuardCounter;

		int heartbeatTime;
		int heartbeatCounter;
		int vignetteCounter;

		int equippedWeapon;
		int targetWeaponEquip;
		
		Vector2 smoothSpeed;
		Vector2 boostVector;

		int boostCooldown;

		SphereCollider axe;

		Game::BasicText playerHealth;
		Game::BasicText shieldHealth;
		Game::BasicText stamina;
		Game::BasicText gunAmmo;

		Game::Sprite lowHPVignette;
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