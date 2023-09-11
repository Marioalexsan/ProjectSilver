#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Medkit.hpp>
#include <ProjectSilver/PlayerPseudoAI.hpp>

namespace Game {
	Medkit::Medkit() :
		Actor(nullptr),
		healthToGrant(0),
		pickUpDelay(30)
	{
		collider.UnregisterFromGame();
		render.SetAlpha(63);
	}

	Medkit::~Medkit() {
	}

	void Medkit::Update() {
		if (pickUpDelay > 0) {
			render.SetAlpha((30 - pickUpDelay) * 192 / 30 + 63);
			pickUpDelay--;
		}
		auto player = Globals::ThePlayer();
		if (player != nullptr && pickUpDelay == 0 && !IsDestructionSignalled() && (player->GetTransform().position - transform.position).Length() < 40.0) {
			auto& stats = ((Actor*)player)->GetStatsReference();
			stats.maxHealth = Utility::ClampValue(stats.maxHealth + healthToGrant, 0.0, 100.0);
			stats.health = Utility::ClampValue(stats.health + int(1.5 * healthToGrant), 0.0, stats.maxHealth);
			
			Globals::Audio().PlaySound("PlayerReload3");
			SignalDestruction();
		}
		Actor::Update();
	}
}