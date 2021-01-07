#include "PCHeader.h"

#include "GameMaster.h"
#include "AI.h"
#include "Actor.h"
#include "PlayerPseudoAI.h"
#include "FighterAI.h"
#include "Actor.h"
#include "FighterBullet.h"
#include "LevelDirector.h"
#include "MenuDirector.h"
#include "KnightAI.h"

namespace Game {
	GameMaster::GameMaster() :
		entityID(1),
		gameRunning(true),
		difficulty(DifficultyLevel::Easy)
	{
	}

	GameMaster::~GameMaster() {
		//Cleanup
		while (entityMasterList.size() > 0) {
			entityMasterList.erase(entityMasterList.begin());
		}

	}

	void GameMaster::AddThePlayer() {
		if (GetThePlayer() != nullptr) {
			return;
		}
		entityMasterList[maxAutoEntityID + SpecialEntities::Player].reset(new Actor(new PlayerPseudoAI()));
		Game::Actor& player = *(Actor*)entityMasterList[maxAutoEntityID + SpecialEntities::Player].get();
		Game::PlayerPseudoAI& playerLogic = *(PlayerPseudoAI*)player.GetAI();
		player.GetComponent().AddAnimation("PlayerIdle");
		player.GetComponent().SetDefaultAnimation("PlayerIdle");
		player.GetComponent().SwitchAnimation("PlayerIdle");
		player.GetComponent().AddAnimation("PlayerShoot");
		player.GetComponent().AddAnimation("CharDead");
		player.GetComponent().AddAnimation("PlayerReload");
		player.GetComponent().AddAnimation("CharShieldUp");
		player.GetComponent().AddAnimation("CharShieldDown");
		player.GetComponent().AddAnimation("CharShieldWalk");
		player.GetComponent().AddAnimation("CharAxeSwing");
		player.GetCollider().SetCombatLayer(Collider::CombatLayer::Players);
		player.GetCollider().SetRadius(40.0);

		player.SetType(EntityType::Player);

		auto& stats = player.GetStatsReference();
		stats.maxHealth = stats.health = 100.0;
		stats.shieldHealth = stats.maxShieldHealth = 35.0;
		stats.stamina = stats.maxStamina = 90.0;
		stats.onHitInvincibilityFrames = 7;
	}

	void GameMaster::RemoveThePlayer() {
		if (GetThePlayer() == nullptr) {
			return;
		}
		entityMasterList.erase(maxAutoEntityID + SpecialEntities::Player);
	}

	Entity* GameMaster::GetThePlayer() {
		uint64_t position = maxAutoEntityID + SpecialEntities::Player;
		if (entityMasterList.find(position) == entityMasterList.end()) {
			return nullptr;
		}
		return entityMasterList[position].get();
	}

	uint64_t GameMaster::AddNewEnemy(EntityType type, Vector2 worldPos) {
		uint64_t ID;
		switch (type) {
		case EntityType::Fighter: {
			ID = NextEntityID();
			entityMasterList[ID].reset(new Actor(new FighterAI()));
			Game::Actor& fighter = *(Actor*)entityMasterList[ID].get();
			fighter.GetComponent().AddAnimation("PlayerIdle");
			fighter.GetComponent().AddAnimation("PlayerShoot");
			fighter.GetComponent().AddAnimation("CharDead");
			fighter.GetComponent().SetDefaultAnimation("PlayerIdle");
			fighter.GetComponent().SwitchAnimation("PlayerIdle");
			fighter.GetCollider().SetCombatLayer(Collider::CombatLayer::Enemies);

			fighter.SetType(EntityType::Fighter);

			auto& stats = fighter.GetStatsReference();
			double HP = 60.0;
			HP += GetDifficulty() * 15.0;
			stats.maxHealth = stats.health = HP;


			fighter.GetTransform().position = worldPos;
		} break;
		case EntityType::FighterBulletProjectile: {
			ID = NextEntityID();
			entityMasterList[ID].reset(new FighterBullet());
			Game::FighterBullet& bullet = *(FighterBullet*)entityMasterList[ID].get();
			bullet.GetComponent().AddAnimation("BulletTravel");
			bullet.GetComponent().SetDefaultAnimation("BulletTravel");
			bullet.GetComponent().SwitchAnimation("BulletTravel");
			bullet.GetCollider().SetEntityDestructionSignalling(true);

			bullet.SetType(EntityType::FighterBulletProjectile);

			bullet.GetTransform().position = worldPos;
		} break;
		case EntityType::Knight: {
			ID = NextEntityID();
			entityMasterList[ID].reset(new Actor(new KnightAI()));
			Game::Actor& knight = *(Actor*)entityMasterList[ID].get();
			knight.GetComponent().AddAnimation("KnightWalk");
			knight.GetComponent().SetDefaultAnimation("KnightWalk");
			knight.GetComponent().SwitchAnimation("KnightWalk");
			knight.GetComponent().AddAnimation("KnightSwing");
			knight.GetComponent().AddAnimation("KnightDead");
			knight.GetCollider().SetCombatLayer(Collider::CombatLayer::Enemies);

			knight.SetType(EntityType::Knight);

			auto& stats = knight.GetStatsReference();
			double HP = 100.0;
			HP += GetDifficulty() * 30.0;
			stats.maxHealth = stats.health = HP;

			knight.GetTransform().position = worldPos;
		} break;
		default:
			std::cout << "Unknown enemy bro!" << endl;
			return 0;
		}
		return ID;
	}

	Entity* GameMaster::GetEntity(uint64_t ID) {
		if (entityMasterList.find(ID) == entityMasterList.end()) {
			return nullptr;
		}
		return entityMasterList[ID].get();
	}

	int GameMaster::GetAliveEnemyCount() {
		int count = 0;
		for (auto& entity : entityMasterList) {
			if (entity.second->GetType() == EntityType::Fighter || entity.second->GetType() == EntityType::Knight) {
				if (((Actor*)entity.second.get())->GetStatsReference().isDead == false) {
					count++;
				}
			}
		}
		return count;
	}

	int GameMaster::GetActiveThreat() {
		int threat = 0;
		for (auto& entity : entityMasterList) {
			if (((Actor*)entity.second.get())->GetStatsReference().isDead == false) {
				switch (entity.second->GetType()) {
				case EntityType::Fighter:
					threat += 15;
					break;
				case EntityType::Knight:
					threat += 40;
					break;
				}
			}
		}
		return threat;
	}

	void GameMaster::RemoveNonSpecialEntities() {
		vector<uint64_t> entitiesToDelete;
		for (auto& pair : entityMasterList) {
			if (pair.first < maxAutoEntityID) {
				entitiesToDelete.push_back(pair.first);
			}
		}

		for (auto& entity : entitiesToDelete) {
			entityMasterList.erase(entity);
		}
	}

	uint64_t GameMaster::NextEntityID() {
		uint64_t returnID = entityID++;
		entityID = (entityID >= maxAutoEntityID ? 1 : entityID);
		return returnID;
	}

	void GameMaster::Update(bool skipGraphicsFrame = false) {
		if (!gameRunning) {
			return;
		}

		Input.Update();

		for (auto& pair : entityMasterList) {
			pair.second->Update();
		}

		BuildSpacialHashMap();
		ResolveMovementCollisions();
		ResolveCombatCollisions();

		vector<uint64_t> entitiesToDelete;
		for (auto& pair : entityMasterList) {
			if (pair.second->IsDestructionSignalled()) {
				entitiesToDelete.push_back(pair.first);
			}
		}

		for (auto& entity : entitiesToDelete) {
			entityMasterList.erase(entity);
		}

		while (colliderUnregisterQueue.size() > 0) {
			RemoveCollider(colliderUnregisterQueue.front());
			colliderUnregisterQueue.pop();
		}
		if (GetThePlayer() != nullptr) {
			Graphics.CenterCameraOn(GetThePlayer()->GetTransform().position);
		}
		if (!skipGraphicsFrame) {
			Graphics.RenderAll();
		}
		Audio.Update();
	}

	void GameMaster::BuildSpacialHashMap() {
		if (spacialHashMap.size() > 0) {
			spacialHashMap.clear();
		}
		for (auto& alpha : colliderLibrary) {
			// Get bounding box of colliders
			BoxCollider* ABox = dynamic_cast<BoxCollider*>(alpha);
			SphereCollider* ASphere = nullptr;
			if (ABox == nullptr) {
				ASphere = dynamic_cast<SphereCollider*>(alpha);
			}

			int startX, endX, startY, endY;

			// Is currently broken, needs fixing
			if (ABox != nullptr || ASphere != nullptr) {
				auto result = alpha->GetBoundingBox();
				startX = floor(result.first.x / cellSize);
				endX = floor(result.second.x / cellSize);
				startY = floor(result.first.y / cellSize);
				endY = floor(result.second.y / cellSize);
			}
			else {
				// Abandon ship
				continue;
			}

			for (int X = startX; X <= endX; X++) {
				for (int Y = startY; Y <= endY; Y++) {
					spacialHashMap[{X, Y}].push_back(alpha);
				}
			}
		}
	}

    #pragma warning(push)
    #pragma warning(disable: 26451)
	vector<Collider*> GameMaster::GetCollisionCandidates(int startX, int startY, int endX, int endY) {
		vector<Collider*> results;
		int prediction = 0;
		for (int X = startX; X <= endX; X++) {
			for (int Y = startY; Y <= endY; Y++) {
				prediction += spacialHashMap[{X, Y}].size();
			}
		}

		
		results.reserve(prediction + 1);
		for (int X = startX; X <= endX; X++) {
			for (int Y = startY; Y <= endY; Y++) {
				for (auto& elem : spacialHashMap[{X, Y}]) {
					// This may generate duplicates
					// If duplicates shouldn't happen, make sure to remove them in destination method
					results.push_back(elem);
				}
			}
		}
		return results;
	}
	#pragma warning(pop)

	void GameMaster::Stop() {
		gameRunning = false;
	}

	bool GameMaster::IsGameRunning() {
		return gameRunning;
	}

	void GameMaster::UltimateMegaInitOfDestiny() {
		// Initializes the assets and stuff

		// Textures
		const string texturePath = "Textures\\";
		Assets.LoadTexture("Checks", texturePath + "Untitled.png");
		Assets.LoadTexture("Test", texturePath + "Untitled2.png");
		Assets.LoadTexture("Char", texturePath + "Char.png");
		Assets.LoadTexture("Test2", texturePath + "Untitled3.png");
		Assets.LoadTexture("LevelFloor", texturePath + "Floor.png");
		Assets.LoadTexture("LevelWall", texturePath + "Walls.png");
		Assets.LoadTexture("CharCircle", texturePath + "CharCircle.png");
		Assets.LoadTexture("CharDead", texturePath + "CharDead.png");
		Assets.LoadTexture("Target", texturePath + "target.png");
		Assets.LoadTexture("CharShoot", texturePath + "CharShoot.png");
		Assets.LoadTexture("Bullet", texturePath + "Bullet.png");
		Assets.LoadTexture("Button", texturePath + "Button.png");
		Assets.LoadTexture("PlayerReload", texturePath + "CharReload.png");
		Assets.LoadTexture("KnightWalk", texturePath + "KnightWalk.png");
		Assets.LoadTexture("KnightSwing", texturePath + "KnightSwing.png");
		Assets.LoadTexture("KnightDead", texturePath + "KnightDead.png");
		Assets.LoadTexture("CharShieldUp", texturePath + "CharShieldUp.png");
		Assets.LoadTexture("CharShieldDown", texturePath + "CharShieldDown.png");
		Assets.LoadTexture("CharShieldWalk", texturePath + "CharShieldWalk.png");
		Assets.LoadTexture("CharAxeSwing", texturePath + "CharAxeSwing.png");
		Assets.LoadTexture("LowHP", texturePath + "LowHP.png");

		// Fonts
		const string fontPath = "Fonts\\";
		Assets.LoadSpriteFont("Huge", fontPath + "CourierNewHuge_0.png", fontPath + "CourierNewHuge.fnt");
		Assets.LoadSpriteFont("Big", fontPath + "CourierNewBig_0.png", fontPath + "CourierNewBig.fnt");


		const string audioPath = "Audio\\";
		// Music
		Assets.LoadMusic("DigitalGhost", audioPath + "digi.ogg");
		Assets.LoadMusicSections("DigitalGhost", {
			{"Loop", {(1 * 60 + 48) * 1000 + 199, (3 * 60 + 1) * 1000 + 566 }}
			});

		Assets.LoadMusic("Menu", audioPath + "menu.ogg");

		// Sound
		Assets.LoadSound("Mooz", audioPath + "Mooz.ogg");
		Assets.LoadSound("PlayerShoot", audioPath + "shot.ogg");
		Assets.LoadSound("PlayerReload1", audioPath + "magout.ogg");
		Assets.LoadSound("PlayerReload2", audioPath + "magin.ogg");
		Assets.LoadSound("PlayerReload3", audioPath + "receiverpull.ogg");
		Assets.LoadSound("GunClick", audioPath + "gunclick.ogg");
		Assets.LoadSound("SwordSwing", audioPath + "swordswing.ogg");
		Assets.LoadSound("ShieldImpact", audioPath + "shieldimpact.ogg");
		Assets.LoadSound("ButtonClick", audioPath + "buttonclick.ogg");
		Assets.LoadSound("Death", audioPath + "death.ogg");
		Assets.LoadSound("PlayerDeath", audioPath + "playerdeath.ogg");
		Assets.LoadSound("PlayerHurt", audioPath + "playerhurt.ogg");
		Assets.LoadSound("HurtBeta", audioPath + "hurt2.ogg");
		Assets.LoadSound("NextWave", audioPath + "nextwave.ogg");
		Assets.LoadSound("Heartbeat", audioPath + "heartbeat.ogg");
		Assets.LoadSound("Boost", audioPath + "boost.ogg");

		// Settings
		Graphics.SetDisplayMode(Graphics.VideoModes.at("1920.1080.f"));
		Input.SetMouseGrab(true);

		// Animation info
		AddAnimation("BoxThing", Game::Animation("Test", {}));
		SetAnimationInfo("BoxThing", { 20, 4, 1, Game::AnimatedSprite::LoopMode::NormalLoop });

		AddAnimation("BoxThing2", Game::Animation("Test2", {}));
		SetAnimationInfo("BoxThing2", { 10, 4, 2, Game::AnimatedSprite::LoopMode::PlayOnce });

		AddAnimation("PlayerIdle", Game::Animation("Char", {}));
		SetAnimationInfo("PlayerIdle", { 1337, 1, 1, Game::AnimatedSprite::LoopMode::Static });
		SetAnimationCenter("PlayerIdle", Vector2(48, 67));


		AddAnimation("CharDead", Game::Animation("CharDead", {}));
		SetAnimationInfo("CharDead", { 1337, 1, 1, Game::AnimatedSprite::LoopMode::Static });
		SetAnimationCenter("CharDead", Vector2(75, 100));

		AddAnimation("BulletTravel", Game::Animation("Bullet", {}));
		SetAnimationInfo("BulletTravel", { 1337, 1, 1, Game::AnimatedSprite::LoopMode::Static });
		SetAnimationCenter("BulletTravel", Vector2(4, 6));

		AddAnimation("PlayerReload", Game::Animation("PlayerReload", {
			{ {Animation::AnimationCriteria::TriggerAtFrameX, "3"}, {Animation::AnimationInstruction::PlaySound, "PlayerReload1"} },
			{ {Animation::AnimationCriteria::TriggerAtFrameX, "8"}, {Animation::AnimationInstruction::PlaySound, "PlayerReload2"} },
			{ {Animation::AnimationCriteria::TriggerAtFrameX, "10"}, {Animation::AnimationInstruction::PlaySound, "PlayerReload3"} }
			}));
		SetAnimationInfo("PlayerReload", { 8, 12, 1, Game::AnimatedSprite::LoopMode::PlayOnce });
		SetAnimationCenter("PlayerReload", Vector2(48, 67));

		using Animation = Game::Animation;

		AddAnimation("PlayerShoot", Animation("CharShoot", { { {Animation::AnimationCriteria::TriggerAtStart, ""}, {Animation::AnimationInstruction::PlaySound, "PlayerShoot"} } }));
		SetAnimationInfo("PlayerShoot", { 4, 4, 1, Game::AnimatedSprite::LoopMode::PlayOnce });
		SetAnimationCenter("PlayerShoot", Vector2(48, 67));

		AddAnimation("KnightDead", Game::Animation("KnightDead", {}));
		SetAnimationInfo("KnightDead", { 1337, 1, 1, Game::AnimatedSprite::LoopMode::Static });
		SetAnimationCenter("KnightDead", Vector2(75, 100));

		AddAnimation("KnightWalk", Animation("KnightWalk", {}));
		SetAnimationInfo("KnightWalk", { 12, 4, 1, Game::AnimatedSprite::LoopMode::NormalLoop });
		SetAnimationCenter("KnightWalk", Vector2(73, 66));

		AddAnimation("KnightSwing", Game::Animation("KnightSwing", {
			{ {Animation::AnimationCriteria::TriggerAtFrameX, "5"}, {Animation::AnimationInstruction::PlaySound, "SwordSwing"} }
			}));
		SetAnimationInfo("KnightSwing", { 6, 12, 1, Game::AnimatedSprite::LoopMode::PlayOnce });
		SetAnimationCenter("KnightSwing", Vector2(73, 116));

		AddAnimation("CharShieldUp", Animation("CharShieldUp", {
			{ {Animation::AnimationCriteria::TriggerAtEnd, ""}, {Animation::AnimationInstruction::SwitchAnimation, "CharShieldWalk"} }
			}));
		SetAnimationInfo("CharShieldUp", { 3, 4, 1, Game::AnimatedSprite::LoopMode::PlayOnce });
		SetAnimationCenter("CharShieldUp", Vector2(73, 66));

		AddAnimation("CharShieldDown", Animation("CharShieldDown", {}));
		SetAnimationInfo("CharShieldDown", { 3, 4, 1, Game::AnimatedSprite::LoopMode::PlayOnce });
		SetAnimationCenter("CharShieldDown", Vector2(73, 66));

		AddAnimation("CharShieldWalk", Animation("CharShieldWalk", {}));
		SetAnimationInfo("CharShieldWalk", { 12, 4, 1, Game::AnimatedSprite::LoopMode::NormalLoop });
		SetAnimationCenter("CharShieldWalk", Vector2(73, 66));

		AddAnimation("CharAxeSwing", Animation("CharAxeSwing", {
			{ {Animation::AnimationCriteria::TriggerAtFrameX, "4"}, {Animation::AnimationInstruction::PlaySound, "SwordSwing"} }
			}));
		SetAnimationInfo("CharAxeSwing", { 5, 12, 1, Game::AnimatedSprite::LoopMode::PlayOnce });
		SetAnimationCenter("CharAxeSwing", Vector2(73, 116));
	}

	void GameMaster::InitLevel() {
		if (entityMasterList.find(maxAutoEntityID + SpecialEntities::TheLevelDirector) != entityMasterList.end()) {
			return;
		}
		entityMasterList[maxAutoEntityID + SpecialEntities::TheLevelDirector].reset(new LevelDirector());
	}

	void GameMaster::UnloadLevel() {
		if (entityMasterList.find(maxAutoEntityID + SpecialEntities::TheLevelDirector) == entityMasterList.end()) {
			return;
		}
		entityMasterList.erase(maxAutoEntityID + SpecialEntities::TheLevelDirector);
	}

	void GameMaster::InitMenu() {
		if (entityMasterList.find(maxAutoEntityID + SpecialEntities::TheMenuDirector) != entityMasterList.end()) {
			return;
		}
		entityMasterList[maxAutoEntityID + SpecialEntities::TheMenuDirector].reset(new MenuDirector());
	}

	void GameMaster::UnloadMenu() {
		if (entityMasterList.find(maxAutoEntityID + SpecialEntities::TheMenuDirector) == entityMasterList.end()) {
			return;
		}
		entityMasterList.erase(maxAutoEntityID + SpecialEntities::TheMenuDirector);
	}

	void GameMaster::AddAnimation(const string& ID, const Animation& animation) {
		if (animationLibrary.find(ID) == animationLibrary.end()) {
			animationLibrary[ID] = animation;
		}
	}

	void GameMaster::SetAnimationInfo(const string& ID, Animation::Info info) {
		if (animationLibrary.find(ID) == animationLibrary.end()) {
			return;
		}
		animationLibrary[ID].SetInfo(info);
	}

	void GameMaster::SetAnimationCenter(const string& ID, const Vector2& center) {
		if (animationLibrary.find(ID) == animationLibrary.end()) {
			return;
		}
		animationLibrary[ID].SetCustomCenter(center);
	}

	void GameMaster::AddCollider(Collider* collider) {
		if (collider != nullptr) {
			colliderLibrary.insert(collider);
		}
	}

	void GameMaster::RemoveCollider(Collider* collider) {
		colliderLibrary.erase(collider);
	}

	void GameMaster::AddColliderToRemovalQueue(Collider* collider) {
		colliderUnregisterQueue.push(collider);
	}

	void GameMaster::ResolveMovementCollisions() {
		using CType = Collider::ColliderType;
		auto libEnd = colliderLibrary.end();
		for (auto firstIt = colliderLibrary.begin(); firstIt != libEnd; firstIt++) {
			auto alpha = *firstIt;
			// Discard pure combat colliders
			if (alpha->GetColliderType() == CType::Combat) {
				continue;
			}

			// Get alpha true type
			BoxCollider* ABox = dynamic_cast<BoxCollider*>(alpha);
			SphereCollider* ASphere = nullptr;
			if (ABox == nullptr) {
				ASphere = dynamic_cast<SphereCollider*>(alpha);
			}

			// TO DO IF NEEDED: Space Hashmap optimization will go after this

			double startX, startY, endX, endY;

			if (ABox != nullptr || ASphere != nullptr) {
				auto result = alpha->GetBoundingBox();
				startX = floor(result.first.x / cellSize);
				endX = floor(result.second.x / cellSize);
				startY = floor(result.first.y / cellSize);
				endY = floor(result.second.y / cellSize);
			}
			else {
				// ABANDON SHIP
				cerr << "Whoop de doo, borkeroo!";
				continue;
			}

			auto candidates = GetCollisionCandidates(int(startX), int(startY), int(endX), int(endY));

			// Reduce second loop to pairs not yet checked by previous loops
			for (auto beta: candidates) {
				// Check prerequisites

				// This may cause multiple checks for certain colliders. But only for a few out of thousands

				// Discard pure combat colliders
				if (beta->GetColliderType() == CType::Combat) {
					continue;
				}

				// Discard if both colliders are static
				if (alpha->GetColliderType() == CType::Static && beta->GetColliderType() == CType::Static) {
					continue;
				}

				// Discard if not a candidate
				/*
				if (std::find(candidates.begin(), candidates.end(), beta->first) == candidates.end()) {
					// Not a candidate
					continue;
				}
				*/
				// Check collision

				BoxCollider* BBox = dynamic_cast<BoxCollider*>(beta);
				SphereCollider* BSphere = nullptr;
				if (BBox == nullptr) {
					BSphere = dynamic_cast<SphereCollider*>(beta);
				}

				pair<bool, Vector2> result = { false, {} };

				// Questionable code here

				// Resolve collisions afterwards

				double APushStr = 0.5;
				double BPushStr = 0.5;

				if (alpha->GetColliderType() == CType::Static) {
					APushStr = 0.0;
					BPushStr = 1.0;
				}
				else if (beta->GetColliderType() == CType::Static) {
					APushStr = 1.0;
					BPushStr = 0.0;
				}

				if (ABox != nullptr) {
					if (BBox != nullptr) {
						result = CollisionMaster::CheckCollision(*ABox, *BBox);
						BBox->Move(result.second * BPushStr);
						ABox->Move(result.second * -APushStr);
					}
					else if (BSphere != nullptr) {
						result = CollisionMaster::CheckCollision(*ABox, *BSphere);
						BSphere->Move(result.second * BPushStr);
						ABox->Move(result.second * -APushStr);
					}
				}
				else if (ASphere != nullptr) {
					if (BBox != nullptr) {
						result = CollisionMaster::CheckCollision(*ASphere, *BBox);
						BBox->Move(result.second * BPushStr);
						ASphere->Move(result.second * -APushStr);
					}
					else if (BSphere != nullptr) {
						result = CollisionMaster::CheckCollision(*ASphere, *BSphere);
						BSphere->Move(result.second * BPushStr);
						ASphere->Move(result.second * -APushStr);
					}
				}
			}
		}
	}

	void GameMaster::ResolveCombatCollisions() {
		using CType = Collider::ColliderType;

		// We must not allow duplicate collisions for combat colliders at ANY cost
		// Duplicates may happen due to imperfect code, unfortunately
		// This solution might be acceptable if we consider that there will be few combat colliders
		// (about one "hitbox" per entity, and perhaps rarely "attackboxes" from enemy attacks)
		set<pair<Collider*, Collider*>> collidedPairs;

		for (auto firstIt = colliderLibrary.begin(); firstIt != colliderLibrary.end(); firstIt++) {
			auto alpha = *firstIt;
			// Discard anything which isn't a combat collider
			if (!(alpha->GetColliderType() == CType::Combat || alpha->GetColliderType() == CType::CombatDynamic)) {
				continue;
			}

			auto AasAttacker = alpha->GetLayersToAttack();
			auto AasDefender = alpha->GetCombatLayer();

			// Get alpha true type
			BoxCollider* ABox = dynamic_cast<BoxCollider*>(alpha);
			SphereCollider* ASphere = nullptr;
			if (ABox == nullptr) {
				ASphere = dynamic_cast<SphereCollider*>(alpha);
			}

			// TO DO IF NEEDED: Space Hashmap optimization will go after this

			double startX, startY, endX, endY;

			if (ABox != nullptr || ASphere != nullptr) {
				auto result = alpha->GetBoundingBox();
				startX = floor(result.first.x / cellSize);
				endX = floor(result.second.x / cellSize);
				startY = floor(result.first.y / cellSize);
				endY = floor(result.second.y / cellSize);
			}
			else {
				// ABANDON SHIP
				cerr << "Whoop de doo, borkeroo!";
				continue;
			}

			auto candidates = GetCollisionCandidates(startX, startY, endX, endY);

			// Reduce second loop to pairs not yet checked by previous loops
			for (auto beta: candidates) {
				// Check prerequisites

				if (collidedPairs.find({ alpha, beta }) != collidedPairs.end() || collidedPairs.find({ beta, alpha }) != collidedPairs.end()) {
					continue;
				}

				// Discard anything which isn't a combat collider
				if (!(beta->GetColliderType() == CType::Combat || beta->GetColliderType() == CType::CombatDynamic)) {
					continue;
				}

				auto BasAttacker = beta->GetLayersToAttack();
				auto BasDefender = beta->GetCombatLayer();

				bool HurtB = false, HurtA = false;
				if (AasAttacker.find(BasDefender) != AasAttacker.end()) {
					HurtB = true;
				}
				if (BasAttacker.find(AasDefender) != BasAttacker.end()) {
					HurtA = true;
				}

				if (!(HurtB || HurtA)) {
					// No point in colliding if they don't target each other
					continue;
				}

				// Check collision

				BoxCollider* BBox = dynamic_cast<BoxCollider*>(beta);
				SphereCollider* BSphere = nullptr;
				if (BBox == nullptr) {
					BSphere = dynamic_cast<SphereCollider*>(beta);
				}

				pair<bool, Vector2> result = { false, {} };

				// Questionable code here

				// Check for collisions without caculating push vectors

				if (ABox != nullptr) {
					if (BBox != nullptr) {
						result = CollisionMaster::CheckCollision(*ABox, *BBox, true);
					}
					else if (BSphere != nullptr) {
						result = CollisionMaster::CheckCollision(*ABox, *BSphere, true);
					}
				}
				else if (ASphere != nullptr) {
					if (BBox != nullptr) {
						result = CollisionMaster::CheckCollision(*ASphere, *BBox, true);
					}
					else if (BSphere != nullptr) {
						result = CollisionMaster::CheckCollision(*ASphere, *BSphere, true);
					}
				}

				if (result.first) {
					// Collision - only Actors can do callbacks
					auto EntityA = dynamic_cast<Game::Actor*>(alpha->GetEntity());
					auto EntityB = dynamic_cast<Game::Actor*>(beta->GetEntity());

					// Disable hurt if a collider's owner entity has been hit before, and the attacker doesn't double hit
					
					if (alpha->GetCollisionOptions().find(Collider::CollisionOptions::DoNotHitRememberedEnemies) != alpha->GetCollisionOptions().end()) {
						auto& AList = alpha->GetHitList();
						auto BEntity = beta->GetEntity();
						if (BEntity != nullptr) {
							if (std::find(AList.begin(), AList.end(), BEntity) != AList.end()) {
								HurtB = false;
							}
							else {
								AList.push_back(BEntity);
							}
						}
					}

					if (beta->GetCollisionOptions().find(Collider::CollisionOptions::DoNotHitRememberedEnemies) != beta->GetCollisionOptions().end()) {
						auto& BList = beta->GetHitList();
						auto AEntity = alpha->GetEntity();
						if (AEntity != nullptr) {
							if (std::find(BList.begin(), BList.end(), AEntity) != BList.end()) {
								HurtA = false;
							}
							else {
								BList.push_back(AEntity);
							}
						}
					}

					if (HurtA) {
						if (EntityA != nullptr && EntityA->GetAI() != nullptr) EntityA->GetAI()->OnHitByAttack(EntityB, beta->GetCombatDamage());
						if (EntityB != nullptr && EntityB->GetAI() != nullptr) EntityB->GetAI()->OnAttackHit();

						//std::cout << "HurtA ";
					}
					if (HurtB) {
						if (EntityB != nullptr && EntityB->GetAI() != nullptr) EntityB->GetAI()->OnHitByAttack(EntityA, alpha->GetCombatDamage());
						if (EntityA != nullptr && EntityA->GetAI() != nullptr) EntityA->GetAI()->OnAttackHit();
						//std::cout << "HurtB ";
					}

					if (alpha->GetCollisionOptions().find(Collider::CollisionOptions::DestroyAfterCombatHit) != alpha->GetCollisionOptions().end()) {
						alpha->SignalDestruction();
					}

					if (beta->GetCollisionOptions().find(Collider::CollisionOptions::DestroyAfterCombatHit) != beta->GetCollisionOptions().end()) {
						beta->SignalDestruction();
					}

					//std::cout << "COL" << alpha->GetCombatDamage() << " " << beta->GetCombatDamage() << endl;
				}
				collidedPairs.insert({ alpha, beta });
			}
		}

		

		// Additional collider checks

		// Destroy combat colliders on contact with Statics
		for (auto firstIt = colliderLibrary.begin(); firstIt != colliderLibrary.end(); firstIt++) {
			auto alpha = *firstIt;

			// Discard anything which isn't a combat collider
			if (!(alpha->GetColliderType() == CType::Combat || alpha->GetColliderType() == CType::CombatDynamic)) {
				continue;
			}

			// Discard if already destroyed from previous steps
			if (alpha->IsDestructionSignalled()) {
				continue;
			}

			auto info = alpha->GetCollisionOptions();

			// Discard anything which doesn't get destroyed on contact with Statics
			if (info.find(Collider::CollisionOptions::DestroyCombatColliderAgainstStatic) == info.end()) {
				continue;
			}

			auto AasAttacker = alpha->GetLayersToAttack();
			auto AasDefender = alpha->GetCombatLayer();

			// Get alpha true type
			BoxCollider* ABox = dynamic_cast<BoxCollider*>(alpha);
			SphereCollider* ASphere = nullptr;
			if (ABox == nullptr) {
				ASphere = dynamic_cast<SphereCollider*>(alpha);
			}

			// TO DO IF NEEDED: Space Hashmap optimization will go after this

			double startX, startY, endX, endY;

			if (ABox != nullptr || ASphere != nullptr) {
				auto result = alpha->GetBoundingBox();
				startX = floor(result.first.x / cellSize);
				endX = floor(result.second.x / cellSize);
				startY = floor(result.first.y / cellSize);
				endY = floor(result.second.y / cellSize);
			}
			else {
				// ABANDON SHIP
				cerr << "Whoop de doo, borkeroo!";
				continue;
			}

			auto candidates = GetCollisionCandidates(startX, startY, endX, endY);

			bool toDestroy = false;

			for (auto beta : candidates) {
				// Check prerequisites

				// This part only checks for static colliders
				if (beta->GetColliderType() != Collider::ColliderType::Static) {
					continue;
				}

				// Check collision

				BoxCollider* BBox = dynamic_cast<BoxCollider*>(beta);
				SphereCollider* BSphere = nullptr;
				if (BBox == nullptr) {
					BSphere = dynamic_cast<SphereCollider*>(beta);
				}

				pair<bool, Vector2> result = { false, {} };

				// Questionable code here
				// Check for collisions without caculating push vectors
				if (ABox != nullptr) {
					if (BBox != nullptr) {
						result = CollisionMaster::CheckCollision(*ABox, *BBox, true);
					}
					else if (BSphere != nullptr) {
						result = CollisionMaster::CheckCollision(*ABox, *BSphere, true);
					}
				}
				else if (ASphere != nullptr) {
					if (BBox != nullptr) {
						result = CollisionMaster::CheckCollision(*ASphere, *BBox, true);
					}
					else if (BSphere != nullptr) {
						result = CollisionMaster::CheckCollision(*ASphere, *BSphere, true);
					}
				}

				if (result.first) {
					// Collision happened. Stop further checks, mark collider for destruction
					toDestroy = true;
					break;
				}
			}

			if (toDestroy) {
				// Collision happened. Destroy the collider
				alpha->SignalDestruction();
			}
		}

	}

	// This function will be extremely slow, unless Spacial Hasmap optimization is implemented
	vector<pair<double, Collider*>> GameMaster::CreateRayCastHitList(const Vector2& A, const Vector2& B) {
		vector<pair<double, Collider*>> hitList;
		using CType = Collider::ColliderType;
		for (auto firstIt = colliderLibrary.begin(); firstIt != colliderLibrary.end(); firstIt++) {
			auto alpha = *firstIt;
			// Get alpha true type
			BoxCollider* ABox = dynamic_cast<BoxCollider*>(alpha);
			SphereCollider* ASphere = nullptr;
			if (ABox == nullptr) {
				ASphere = dynamic_cast<SphereCollider*>(alpha);
			}

			pair<bool, double> result = { false, 0.0 };

			if (ABox != nullptr) {
				result = CollisionMaster::RaycastVSBox(A, B, *ABox);
			}
			else if (ASphere != nullptr) {
				result = CollisionMaster::RaycastVSSphere(A, B, *ASphere);
			}

			if (result.first) {
				// This one collides
				// Add to hit list
				hitList.push_back({ result.second, alpha });
			}
		}

		// Sort based on distance. Uses a lambda KEKLOL
		std::sort(hitList.begin(), hitList.end(), [](auto& left, auto& right) {
			return left.first < right.first;
			});

		return hitList;
	}
}