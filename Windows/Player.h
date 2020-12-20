#ifndef PLAYER_HEADER
#define PLAYER_HEADER

#include "PCHeader.h"

#include "Actor.h"

namespace Game {
	class Player: public Actor {
	public:
		struct Weapon {
			int magSize;
			int maxAmmo;
			bool bulletInChamber; // Can chamber a round

			int bulletsPerShot; // default 1. Is higher for shotguns

			int reloadTime; // In frames
			int tacticalReloadTime; // In frames, for guns that have a chambered round

			double inaccuracy; // arc in which a shot might be fired when standing still
			double movementInaccuracyPerUnit; // added arc if moving, per speed unit
			double shotRecoil; // added recoil after a shot

			int equipTime; // time before a gun can be fired after being equipped

			int fireCooldown; // cooldown in between shots

			int currentAmmo;
			int currentMag;
			int currentRecoil;
			bool bulletChambered;

			bool autoFire;

			double damage;
		};
	protected:

		vector<Weapon> weapons;
		int equippedWeapon;
		int weaponDelay;
		int reloadDelay;
		double recoil;
		
		void DissipateRecoilStep();
		void DoTheReload();
	public:
		Player();

		bool FireGun();
		bool Reload();

		virtual void Update();

		int GetCurrentMag();
		int GetCurrentAmmo();
		bool GetChamberedBullet();
		bool CanChamberBullet();
	};
}

#endif