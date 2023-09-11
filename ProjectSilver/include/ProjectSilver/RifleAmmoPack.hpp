#pragma once

#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Actor.hpp>

namespace Game {
	class RifleAmmoPack : public Actor {
		int pickUpDelay;
		int ammoToGrant;
	public:
		inline void SetAmmoToGrant(int ammo) { this->ammoToGrant = ammo; }

		RifleAmmoPack();
		~RifleAmmoPack();

		virtual void Update();
	};
}

