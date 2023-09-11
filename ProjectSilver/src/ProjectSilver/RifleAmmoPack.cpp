#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/RifleAmmoPack.hpp>
#include <ProjectSilver/PlayerPseudoAI.hpp>

namespace Game {
	RifleAmmoPack::RifleAmmoPack() :
		Actor(nullptr),
		ammoToGrant(0),
		pickUpDelay(30)
	{
		collider.UnregisterFromGame();
		render.SetAlpha(63);
	}

	RifleAmmoPack::~RifleAmmoPack() {
	}

	void RifleAmmoPack::Update() {
		if (pickUpDelay > 0) {
			render.SetAlpha((30 - pickUpDelay) * 192 / 30 + 63);
			pickUpDelay--;
		}
		auto player = Globals::ThePlayer();
		if (player != nullptr  && !IsDestructionSignalled() && (player->GetTransform().position - transform.position).Length() < 40.0) {
			((PlayerPseudoAI*)((Actor*)player)->GetAI())->AddRifleAmmo(ammoToGrant);
			Globals::Audio().PlaySound("PlayerReload3");
			SignalDestruction();
		}
		Actor::Update();
	}
}