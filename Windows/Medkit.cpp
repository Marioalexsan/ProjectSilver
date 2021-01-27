#include "PCHeader.h"
#include "Medkit.h"
#include "PlayerPseudoAI.h"

namespace Game {
	Medkit::Medkit() :
		Actor(nullptr),
		healthToGrant(0),
		pickUpDelay(30)
	{
		collider.UnregisterFromGame();
	}

	Medkit::~Medkit() {
	}

	void Medkit::Update() {
		if (pickUpDelay > 0) {
			pickUpDelay--;
		}
		if (pickUpDelay == 0 && !IsDestructionSignalled() && (Globals::ThePlayer()->GetTransform().position - transform.position).Length() < 40.0) {
			auto& stats = ((Actor*)Globals::ThePlayer())->GetStatsReference();
			stats.maxHealth = Utility::ClampValue(stats.maxHealth + healthToGrant, 0.0, 100.0);
			stats.health = Utility::ClampValue(stats.health + 2 * healthToGrant, 0.0, stats.maxHealth);
			
			Globals::Audio().PlaySound("PlayerReload3");
			SignalDestruction();
		}
		Actor::Update();
	}
}