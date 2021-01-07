#ifndef GAME_HEADER
#define GAME_HEADER

#include "AssetManager.h"
#include "AudioEngine.h"
#include "GraphicsEngine.h"
#include "InputHandler.h"
#include "Animation.h"
#include "CollisionMaster.h"

namespace Game {
	class Entity;

	class Actor;
	enum EntityType;

	class GameMaster {
	public:
		enum SpecialEntities {
			Player = 1,
			TheLevelDirector = 2,
			TheMenuDirector = 3
		};

		enum DifficultyLevel {
			Easy = 0,
			Normal = 1,
			Hard = 2
		};

	private:
		#pragma region Temporary Stuff for testing

		#pragma endregion

		bool gameRunning;

		map<string, Animation> animationLibrary;
		set<Collider*> colliderLibrary;
		queue<Collider*> colliderUnregisterQueue;

		double cellSize = 100.0;
		map<pair<int, int>, vector<Collider*>> spacialHashMap;

		map<uint64_t, std::unique_ptr<Entity>> entityMasterList;
		uint64_t entityID;
		static const uint64_t maxAutoEntityID = 16384;

		uint64_t NextEntityID();

		DifficultyLevel difficulty;

	public:
		inline void SetDifficulty(DifficultyLevel difficulty) { this->difficulty = difficulty; }

		inline DifficultyLevel GetDifficulty() { return this->difficulty; }

		Entity* GetThePlayer();
		void AddThePlayer();
		void RemoveThePlayer();

		uint64_t AddNewEnemy(EntityType type, Vector2 worldPos);
		Entity* GetEntity(uint64_t ID);
		int GetAliveEnemyCount();

		int GetActiveThreat();

		void RemoveNonSpecialEntities();

		void BuildSpacialHashMap();
		vector<Collider*> GetCollisionCandidates(int startX, int startY, int endX, int endY);

		const double fixedTimeStep = 16.67;
		const int maxFrameRate = 60;

		Game::AudioEngine Audio;
		Game::GraphicsEngine Graphics;
		Game::AssetManager Assets;
		Game::InputHandler Input;

		GameMaster();
		~GameMaster();

		void UltimateMegaInitOfDestiny();

		void InitLevel();
		void UnloadLevel();

		void InitMenu();
		void UnloadMenu();

		void Update(bool skipGraphicsFrame);
		void Stop();
		bool IsGameRunning();

		void AddAnimation(const string& ID, const Animation& animation);

		void SetAnimationInfo(const string& ID, Animation::Info info);

		void SetAnimationCenter(const string& ID, const Vector2& center);

		inline const map<string, Animation>& GetAnimationLibrary() {
			return animationLibrary;
		}

		void AddCollider(Collider* collider);
		void RemoveCollider(Collider* collider);
		void AddColliderToRemovalQueue(Collider* collider);

		void ResolveMovementCollisions();
		void ResolveCombatCollisions();

		vector<pair<double, Collider*>> CreateRayCastHitList(const Vector2& A, const Vector2& B);
	};
}

#endif
