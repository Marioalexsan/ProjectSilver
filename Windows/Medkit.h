#ifndef MEDKIT_HEADER
#define MEDKIT_HEADER

#include "PCHeader.h"
#include "Actor.h"

namespace Game {
	class Medkit : public Actor {
		int pickUpDelay;
		int healthToGrant;
	public:
		inline void SetHealthToGrant(int hp) { this->healthToGrant = hp; }

		Medkit();
		~Medkit();

		virtual void Update();
	};
}

#endif
