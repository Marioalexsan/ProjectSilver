#ifndef GAME_HEADER
#define GAME_HEADER

#include "PCHeader.h"

#include "AssetManager.h"
#include "AudioEngine.h"
#include "GraphicsEngine.h"
#include "InputHandler.h"
#include "Animation.h"
#include "CollisionMaster.h"
#include "Effect.h"



namespace Game {
	class Entity;

	class Actor;
	enum class EntityType;

	class GameMaster {
	public:
		enum SpecialEntities {
			Player = 1,
			TheLevelDirector = 2,
			TheMenuDirector = 3,
			Shadow = 4,
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
		
		map<uint64_t, Collider*> colliderLibrary;
		uint64_t colliderID;
		static const uint64_t maxAutoColliderID = 1000000;
		
		queue<uint64_t> colliderUnregisterQueue;

		double cellSize = 200.0;
		map<pair<int, int>, set<Collider*>> spatialHashMap;

		map<uint64_t, std::unique_ptr<Entity>> entityMasterList;
		uint64_t entityID;
		static const uint64_t maxAutoEntityID = 1000000;

		map<uint64_t, std::unique_ptr<Effect>> effectMasterList;
		uint64_t effectID;
		static const uint64_t maxAutoEffectID = 1000000;

		uint64_t NextEntityID();
		uint64_t NextEffectID();
		uint64_t NextColliderID();

		DifficultyLevel difficulty;

		int renderDisableCounter;

	public:
		// For avoiding epilepsy during fullscreen change
		inline void SetRenderDisableCounter(int count) { renderDisableCounter = count; }

		inline void SetDifficulty(DifficultyLevel difficulty) { this->difficulty = difficulty; }

		inline DifficultyLevel GetDifficulty() { return this->difficulty; }

		Entity* GetThePlayer();
		void AddThePlayer();
		void RemoveThePlayer();

		Entity* GetTheShadow();
		void AddTheShadow();
		void RemoveTheShadow();

		uint64_t AddEntity(EntityType type, Vector2 worldPos);
		Entity* GetEntity(uint64_t ID);
		int GetAliveEnemyCount();

		int GetEntityCountOfType(EntityType searchType);

		uint64_t CreateDefaultTracerEffect(const Vector2& start, const Vector2& end);
		uint64_t CreateAfterImageEffect(const Transform& parentTransform, const string& texAnimID, int frame = -1);

		int GetActiveThreat();

		void RemoveNonSpecialEntities();
		void ClearEffects();

		void BuildSpatialHashMap();
		vector<Collider*> GetCollisionCandidates(int startX, int startY, int endX, int endY);

		const int fixedTimeStepInMilliseconds = 17;
		const int maxFrameRate = 60;

		Game::AudioEngine Audio;
		Game::GraphicsEngine Graphics;
		Game::AssetManager Assets;
		Game::InputHandler Input;

		GameMaster();
		~GameMaster();

		void UltimateMegaInitOfDestiny();
		void ArmageddonExitProcedures();

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

		uint64_t AddCollider(Collider* collider);
		bool RemoveCollider(uint64_t);
		void AddColliderToRemovalQueue(uint64_t ID);

		void ResolveMovementCollisions();
		void ResolveCombatCollisions();

		vector<pair<double, Collider*>> CreateRayCastHitList(const Vector2& A, const Vector2& B);
	};
}

#endif
