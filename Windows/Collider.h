#ifndef COLLIDER_HEADER
#define COLLIDER_HEADER

#include "PCHeader.h"
#include "MiscUtility.h"



namespace Game {
	class Entity;

	class Collider {
	public:
		enum ColliderType {
			Static,
			Dynamic,
			CombatDynamic,
			Combat,
			CombatStatic
		};

		enum CombatLayer {
			None,
			Players,
			Enemies
		};

		enum CollisionOptions {
			DestroyCombatColliderAgainstStatic,
			DestroyAfterCombatHit,
			DoNotHitRememberedEntities
		};

	protected:
		Trackable<Game::Transform> transform; // Does not use the center and rotation, but I've opted for easier tracking of Entity transform
		double mass; // Mass of 0.0 means that it cannot be moved in generic conditions

		ColliderType type;
		CombatLayer combatLayer;
		set<CombatLayer> attackerLayers;
		set<CollisionOptions> info;

		Entity* owner;

		double damage;

		vector<Entity*> hitEntities;

		bool destructionSignalled;
		bool alsoSignalEntityDestruction;
	public:
		Collider();
		Collider(const Vector2& position, ColliderType type);

		virtual ~Collider();

		Trackable<Game::Transform>& GetTransform();

		virtual const Vector2& GetPosition() const;
		void SetPosition(const Vector2& position);
		void Move(const Vector2& amount);

		inline void SetEntityDestructionSignalling(bool mode) {
			alsoSignalEntityDestruction = mode;
		}

		inline vector<Entity*>& GetHitList() { return hitEntities; }

		void SignalDestruction();

		inline bool IsDestructionSignalled() { return destructionSignalled; }

		inline void SetOwner(Entity* en) { owner = en; }

		inline Entity* GetEntity() { return owner; }

		inline void SetCombatDamage(double damage) { this->damage = damage; }

		inline double GetCombatDamage() { return damage; }

		inline void SetCombatLayer(CombatLayer layer) { combatLayer = layer; }

		inline CombatLayer GetCombatLayer() { return combatLayer; }

		inline void SetLayersToAttack(set<CombatLayer> setLayers) { attackerLayers = setLayers; }

		inline const set<CombatLayer>& GetLayersToAttack() { return attackerLayers; }

		inline void SetCollisionOptions(set<CollisionOptions> setInfo) { info = setInfo; }

		inline const set<CollisionOptions>& GetCollisionOptions() { return info; }

		inline void SetColliderType(ColliderType type) { this->type = type; }

		inline ColliderType GetColliderType() { return type; }

		virtual pair<Vector2, Vector2> GetBoundingBox() = 0;

		void RegisterToGame();

		void UnregisterFromGame();

		void QueueUnregisterFromGame();
	};
}

#endif