#include "PCHeader.h"
#include "RifleAmmoPack.h"
#include "PlayerPseudoAI.h"

namespace Game {
	RifleAmmoPack::RifleAmmoPack() :
		Actor(nullptr),
		ammoToGrant(0),
		pickUpDelay(30)
	{
		collider.UnregisterFromGame();
	}

	RifleAmmoPack::~RifleAmmoPack() {
	}

	void RifleAmmoPack::Update() {
		if (pickUpDelay > 0) {
			pickUpDelay--;
		}
		if (pickUpDelay == 0 && !IsDestructionSignalled() && (Globals::ThePlayer()->GetTransform().position - transform.position).Length() < 40.0) {
			((PlayerPseudoAI*)((Actor*)Globals::ThePlayer())->GetAI())->AddRifleAmmo(ammoToGrant);
			Globals::Audio().PlaySound("PlayerReload3");
			SignalDestruction();
		}
		Actor::Update();
	}
}