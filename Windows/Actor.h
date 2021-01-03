#ifndef ACTOR_HEADER
#define ACTOR_HEADER

#include "PCHeader.h"
#include "Entity.h"
#include "RenderComponent.h"

namespace Game {
	class AI;
	
	class Actor : public Entity {
	public:
		struct ActorStats {
			double health;
			double maxHealth;
			double stamina;
			double maxStamina;
			double baseDamage;

			double knockbackResistance; // 0-100%

			double movementSpeed; // units moved per second

			bool invulnerable;
			bool isDead;

			int currentInvincibilityFrames;
			int onHitInvincibilityFrames;
		};

		enum ActorType {
			Unknown,
			Player,
			Enemy,
			NonSentient // Spells, projectiles, etc.
		};
	protected:
		ActorStats baseStats;
		Vector2 knockback;

		SphereCollider collider;
		RenderComponent render;

		AI* brain;

		void ApplyKnockbackStep();
		void ApplyKnockback();
	public:
		Actor(AI* brain);
		//Actor();
		~Actor();

		inline AI* GetAI() {
			return brain;
		}

		inline SphereCollider& GetCollider() { return collider; }

		inline RenderComponent& GetComponent() { return render; }
		
		void Move(Vector2 vector);
		void MoveForward(double amount);
		ActorStats& GetStatsReference();

		virtual void Update();
	};
}

#endif