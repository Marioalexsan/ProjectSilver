#pragma once

#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Actor.hpp>

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

