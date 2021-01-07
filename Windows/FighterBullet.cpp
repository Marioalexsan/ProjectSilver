#include "PCHeader.h"
#include "FighterBullet.h"

namespace Game {
	FighterBullet::FighterBullet() :
		Actor(nullptr),
		distancePerFrame(8.0 + Globals::Difficulty() * 2.5),
		currentDistance(0.0),
		maxDistance(1800.0)
	{
		collider.SetCollisionOptions({Collider::CollisionOptions::DestroyCombatColliderAgainstStatic, Collider::CollisionOptions::DestroyAfterCombatHit });
		collider.SetColliderType(Collider::Combat);
		collider.SetCombatLayer(Collider::CombatLayer::None);
		collider.SetLayersToAttack({ Collider::CombatLayer::Players });
		collider.SetCombatDamage(15.0 + Globals::Difficulty());
		collider.SetRadius(6);
	}

	FighterBullet::~FighterBullet() {

	}

	void FighterBullet::Update() {
		if (collider.IsDestructionSignalled()) {
			SignalDestruction();
			return;
		}
		MoveForward(distancePerFrame);
		currentDistance += distancePerFrame;
		if (currentDistance >= maxDistance) {
			SignalDestruction();
		}
		Actor::Update();
	}
}