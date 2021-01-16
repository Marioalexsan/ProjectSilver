#include "PCHeader.h"
#include "Effect.h"

namespace Game {
	Effect::Effect():
		timeToLive(0)
	{}

	void Effect::Update() {
		if (timeToLive > 0) {
			timeToLive--;
		}
	}
}