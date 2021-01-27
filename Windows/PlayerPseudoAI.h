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
		int shieldRegenCounter;
		int shieldFadeOutDelay;
		int perfectGuardCounter;
		bool hasPerfectGuarded;

		bool doingSwing;

		bool wasInWeaponSwitch;

		int regenCounter;
		
		int staminaRegenCounter;
		int staminaFadeOutDelay;
		int boostCooldown;

		int heartbeatTime;
		int heartbeatCounter;
		int vignetteCounter;

		int equippedWeapon;
		int targetWeaponEquip;
		int pistolPenaltyCountdown;
		
		Vector2 smoothSpeed;
		Vector2 boostVector;


		SphereCollider axe;

		Game::BasicText playerHealth;
		Game::BasicText shieldHealth;
		Game::BasicText stamina;
		Game::BasicText gunAmmo;

		Game::Sprite lowHPVignette;

		void GenericWeaponFireLogic(double damageToDeal, double angleDeltaToApply = 0.0);
	public:
		PlayerPseudoAI();
		~PlayerPseudoAI();

		inline void AddRifleAmmo(int ammo) {
			rifleAmmoPool += ammo;
			if (rifleAmmoPool + currentRifleAmmo > 60) {
				rifleAmmoPool = 60 - currentRifleAmmo;
			}
		}

		virtual void OnAttackHit();
		virtual void OnHitByAttack(Actor* attacker, double damage);

		virtual void Update();

		virtual void OnDeath();
	};
}

#endif