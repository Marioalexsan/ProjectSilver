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
	enum ActorType;

	class GameMaster {
	public:
		enum SpecialEntities {
			Player = 1
		};

	private:
		#pragma region Temporary Stuff for testing

		#pragma endregion

		bool gameRunning;

		map<string, Animation> animationLibrary;
		set<Collider*> colliderLibrary;

		double cellSize = 100.0;
		map<pair<int, int>, vector<Collider*>> spacialHashMap;

		map<uint64_t, std::unique_ptr<Entity>> entityMasterList;
		uint64_t entityID;
		static const uint64_t maxAutoEntityID = 16384;

		uint64_t NextEntityID();

	public:
		Entity* GetThePlayer();
		void AddThePlayer();

		uint64_t AddNewEnemy(ActorType type, Vector2 worldPos);

		void BuildSpacialHashMap();
		vector<Collider*> GetCollisionCandidates(int startX, int startY, int endX, int endY);

		const double fixedTimeStep = 16.67;
		const int maxFrameRate = 60;

		Game::AudioEngine Audio;
		Game::GraphicsEngine Graphics;
		Game::AssetManager Assets;
		Game::InputHandler Input;

		GameMaster();

		void UltimateMegaInitOfDestiny();

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

		void ResolveMovementCollisions();
		void ResolveCombatCollisions();

		vector<pair<double, Collider*>> CreateRayCastHitList(const Vector2& A, const Vector2& B);
	};
}

#endif
