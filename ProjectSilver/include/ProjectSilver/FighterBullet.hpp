#pragma once

#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Actor.hpp>

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

