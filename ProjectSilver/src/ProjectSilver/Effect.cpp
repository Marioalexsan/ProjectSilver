#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Effect.hpp>

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