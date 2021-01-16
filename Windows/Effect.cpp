#include "PCHeader.h"
#include "Effect.h"

namespace Game {
	void Effect::Update() {
		if (timeToLive > 0) {
			timeToLive--;
		}
	}
}