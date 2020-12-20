#ifndef ACTOR_HEADER
#define ACTOR_HEADER

#include "PCHeader.h"
#include "Entity.h"

namespace Game {
	
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
		};

		enum ActorType {
			Unknown,
			Player,
			Enemy,
			Process // Spells, projectiles, etc.
		};
	protected:
		ActorStats baseStats;
		Vector2 knockback;

		void ApplyKnockbackStep();
	public:
		
		void Move(Vector2 vector);
		void MoveForward(double amount);
		ActorStats& GetStatsReference();

		virtual void Update();
		virtual void OnAttackHit() = 0;
		virtual void OnHitByAttack() = 0;
		virtual void OnDeathOverride() = 0;
	};
}

#endif