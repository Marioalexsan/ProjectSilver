#ifndef AMMOPACK_HEADER
#define AMMOPACK_HEADER

#include "PCHeader.h"
#include "Actor.h"

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

#endif
