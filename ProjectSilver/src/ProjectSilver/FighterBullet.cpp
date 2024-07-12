#include <ProjectSilver/FighterBullet.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    FighterBullet::FighterBullet() :
    Actor(nullptr),
    distancePerFrame(8.0 + Globals::Difficulty() * 2.5),
    currentDistance(0.0),
    maxDistance(1800.0)
    {
        collider.SetCollisionOptions(
            {Collider::CollisionOptions::DestroyCombatColliderAgainstStatic,
             Collider::CollisionOptions::DestroyAfterCombatHit});
        collider.SetColliderType(Collider::Combat);
        collider.SetCombatLayer(Collider::CombatLayer::None);
        collider.SetLayersToAttack({Collider::CombatLayer::Players});
        collider.SetCombatDamage(16.0 + 2.0 * Globals::Difficulty());
        collider.SetRadius(6);
    }

    FighterBullet::~FighterBullet()
    {
    }

    void FighterBullet::Update()
    {
        if (collider.IsDestructionSignalled())
        {
            collider.QueueUnregisterFromGame();
            SignalDestruction();
            return;
        }
        MoveForward(distancePerFrame);
        currentDistance += distancePerFrame;
        if (currentDistance >= maxDistance)
        {
            SignalDestruction();
        }
        Globals::Game().CreateAfterImageEffect(transform, "Bullet_Travel", 0);
        Actor::Update();
    }
} // namespace Game