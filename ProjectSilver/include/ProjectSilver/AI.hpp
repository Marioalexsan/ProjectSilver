#pragma once

#include <ProjectSilver/MiscUtility.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    class Actor;
    class Entity;

    class AI
    {
    protected:
        Actor* entity;

        Vector2 smoothSpeed;

        int counter;
        int disableUpdatesCounter;
        int destroyDelay;

        bool EntityIsDeadAF();
        bool ProcessGenericDestroyDelay();

        bool DelayedSpawningLogic();
        bool PlayerIsUnavailable();
        bool HasPlayerLineOfSight();
        bool HasLineOfSight(Vector2 point);

    public:
        AI(Actor* owner);
        AI();

        virtual ~AI();

        inline void SetEntity(Actor* entity)
        {
            this->entity = entity;
        }

        inline Actor* GetEntity()
        {
            return entity;
        }

        virtual void OnAttackHit()                                 = 0;
        virtual void OnHitByAttack(Actor* attacker, double damage) = 0;
        virtual void OnDeath();
        virtual void Update() = 0;
    };
} // namespace Game
