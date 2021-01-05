#ifndef FIGHTERBULLET_HEADER
#define FIGHTERBULLET_HEADER

#include "PCHeader.h"
#include "Actor.h"

namespace Game {
	class FighterBullet : public Actor {
		double distancePerFrame;
		double currentDistance;
		double maxDistance;
	public:
		FighterBullet();
		~FighterBullet();

		virtual void Update();
	};
}

#endif
