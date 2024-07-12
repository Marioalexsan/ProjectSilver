#include <ProjectSilver/Collider.hpp>
#include <ProjectSilver/Entity.hpp>
#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    Collider::Collider() :
    destructionSignalled(false),
    alsoSignalEntityDestruction(false),
    transform(),
    mass(80.0),
    owner(nullptr),
    type(Static),
    damage(0.0),
    combatLayer(None),
    colliderGameMasterID(0)
    {
    }

    Collider::Collider(const Vector2& position, ColliderType type) :
    destructionSignalled(false),
    alsoSignalEntityDestruction(false),
    transform(),
    mass(80.0),
    owner(nullptr),
    type(type),
    damage(0.0),
    combatLayer(None),
    colliderGameMasterID(0)
    {
        transform->position = position;
    }

    Collider::~Collider()
    {
        // Make sure we don't have dangling pointers
        UnregisterFromGame();
    }

    Trackable<Transform>& Collider::GetTransform()
    {
        return transform;
    }

    const Game::Vector2& Collider::GetPosition() const
    {
        return transform->position;
    }

    void Collider::SetPosition(const Vector2& position)
    {
        transform->position = position;
    }

    void Collider::Move(const Vector2& amount)
    {
        transform->position += amount;
    }

    void Collider::SignalDestruction()
    {
        if (info.find(Collider::CollisionOptions::DestroyCombatColliderAgainstStatic) !=
            info.end())
        {
            destructionSignalled = true;
            if (alsoSignalEntityDestruction && owner != nullptr)
            {
                owner->SignalDestruction();
            }
        }
    }

    void Collider::RegisterToGame()
    {
        if (colliderGameMasterID == 0)
        {
            colliderGameMasterID = Globals::Game().AddCollider(this);
        }
    }

    void Collider::UnregisterFromGame()
    {
        Globals::Game().RemoveCollider(colliderGameMasterID);
        colliderGameMasterID = 0;
    }

    void Collider::QueueUnregisterFromGame()
    {
        Globals::Game().AddColliderToRemovalQueue(colliderGameMasterID);
    }
} // namespace Game