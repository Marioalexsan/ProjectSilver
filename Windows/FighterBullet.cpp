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
		collider.SetCombatLayer(Collider::CombatLayer::Enemies);
		collider.SetLayersToAttack({ Collider::CombatLayer::Players });
		collider.SetCombatDamage(20.0);
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