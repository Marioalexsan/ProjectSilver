#include "PCHeader.h"

#include "Player.h"
#include "Globals.h"

namespace Game {
	Player::Player():
		Actor(nullptr) {
		AssetManager* manager = &Globals::Assets();

		weapons.push_back({});
		Weapon* test = &weapons[0];
		test->magSize = 6;
		test->maxAmmo = 900;
		test->bulletInChamber = false;
		test->bulletsPerShot = 1;
		test->reloadTime = 240;
		test->tacticalReloadTime = 240;
		test->inaccuracy = 1;
		test->movementInaccuracyPerUnit = 0.02;
		test->shotRecoil = 60;

		test->equipTime = 60;
		test->fireCooldown = 24;
		test->currentAmmo = test->maxAmmo;
		test->currentMag = test->magSize;

		test->bulletChambered = false;

		test->damage = 30;
		test->autoFire = false;

		equippedWeapon = 0;
		weaponDelay = 0;
		recoil = 0.0;
	}

	void Player::DissipateRecoilStep() {
		recoil = recoil * 0.92;
		if (recoil < 0.5) {
			recoil = 0;
		}
	}

	bool Player::FireGun() {
		auto* data = &weapons[equippedWeapon];
		if (weaponDelay > 0 || reloadDelay > 0 || data->currentAmmo <= 0) {
			return false;
		}
		data->currentAmmo--;
		weaponDelay = data->fireCooldown;

		return true;
	}

	bool Player::Reload() {
		auto* data = &weapons[equippedWeapon];
		if (weaponDelay > 0 || reloadDelay > 0 || !(data->currentMag < data->magSize || data->bulletInChamber == true && data->bulletChambered == false) || (data->currentAmmo <= 0)) {
			return false;
		}
		reloadDelay = data->reloadTime;

		return true;
	}

	void Player::DoTheReload() {
		auto* data = &weapons[equippedWeapon];
		if (data->bulletInChamber) {
			data->bulletChambered = true;
			data->currentAmmo--;
		}
		int missingMag = data->magSize - data->currentMag;
		int spendableAmmo = (data->currentAmmo < missingMag) ? data->currentAmmo : missingMag;
		data->currentMag += spendableAmmo;
		data->currentAmmo -= spendableAmmo;
	}

	void Player::Update() {
		DissipateRecoilStep();
		if(reloadDelay > 0) reloadDelay--;
		if(weaponDelay > 0) weaponDelay--;
		if (reloadDelay == 0) {
			DoTheReload();
		}
	}


	int Player::GetCurrentMag() {
		return weapons[equippedWeapon].currentMag;
	}
	int Player::GetCurrentAmmo() {
		return weapons[equippedWeapon].currentAmmo;
	}
	bool Player::GetChamberedBullet() {
		return weapons[equippedWeapon].bulletChambered;
	}
	bool Player::CanChamberBullet() {
		return weapons[equippedWeapon].bulletInChamber;
	}
}