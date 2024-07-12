#pragma once

#include <ProjectSilver/Entity.hpp>
#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/RenderComponent.hpp>

namespace Game
{
    class AI;

    class Actor : public Entity
    {
    public:
        struct ActorStats
        {
            double health;
            double maxHealth;
            double stamina;
            double maxStamina;

            double shieldHealth;
            double maxShieldHealth;

            double knockbackResistance; // UNIMPLEMENTED

            bool invulnerable;
            bool isDead;

            int currentInvincibilityFrames;
            int onHitInvincibilityFrames;
        };

    protected:
        ActorStats baseStats;
        Vector2    knockback;

        SphereCollider  collider;
        RenderComponent render;

        AI* brain;

        void ApplyKnockbackStep();
        void ApplyKnockback();

    public:
        Actor(AI* brain);
        //Actor();
        ~Actor();

        inline AI* GetAI()
        {
            return brain;
        }

        inline SphereCollider& GetCollider()
        {
            return collider;
        }

        inline RenderComponent& GetComponent()
        {
            return render;
        }

        void        Move(Vector2 vector);
        void        MoveForward(double amount);
        ActorStats& GetStatsReference();

        void Rotate(double amount);

        // Rotate on shortest path
        void RotateTowardsDirection(double amount, double targetDirection);

        virtual void Update();
    };
} // namespace Game
