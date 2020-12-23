#ifndef COLLIDER_HEADER
#define COLLIDER_HEADER

#include "PCHeader.h"
#include "MiscUtility.h"

namespace Game {
	class Collider {
	public:
		enum ColliderOwners {
			Player,
			Enemy,
			Environment
		};
		enum ColliderTypes {
			Static,
			Dynamic,
			Field,
			Trigger
		};
		enum ColliderCombatRole {
			Generic,
			Attacker,
			Defender
		};
		struct ColliderInfo {
			ColliderOwners ownerType;
			ColliderTypes colType;
			ColliderCombatRole combatType;
		};

	protected:
		Vector2 position;
		double mass; // Mass of 0.0 means that it cannot be moved in generic conditions
		ColliderInfo info;
	public:
		Collider();
		Collider(const Vector2& position, double mass);

		void SetSelectivity(ColliderOwners owner, ColliderTypes type, ColliderCombatRole role);
		ColliderInfo GetSelectivity();

		virtual ~Collider() = default; // Force virtualization

		const Vector2& GetPosition() const;
		void SetPosition(const Vector2& position);
		void Move(const Vector2& amount);
	};
}

#endif