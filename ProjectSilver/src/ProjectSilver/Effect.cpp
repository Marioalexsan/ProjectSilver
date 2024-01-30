module;

#include <ProjectSilver/PCHeader.hpp>

module ProjectSilver.Effect;

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