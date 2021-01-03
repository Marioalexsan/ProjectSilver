#include "PCHeader.h"

#include "GameMaster.h"
#include "AI.h"
#include "Actor.h"
#include "PlayerPseudoAI.h"

namespace Game {
	GameMaster::GameMaster():
		entityID(1),
		gameRunning(true)
	{
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
		player.GetComponent().SetCenter({ 48, 67 });
	}

	Entity* GameMaster::GetThePlayer() {
		uint64_t position = maxAutoEntityID + SpecialEntities::Player;
		if (entityMasterList.find(position) == entityMasterList.end()) {
			return nullptr;
		}
		return entityMasterList[position].get();
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

			for(int X = startX; X <= endX; X++) {
				for(int Y = startY; Y <= endY; Y++) {
					spacialHashMap[{X, Y}].push_back(alpha);
				}
			}
		}
	}

	vector<Collider*> GameMaster::GetCollisionCandidates(int startX, int startY, int endX, int endY) {
		vector<Collider*> results;
		int prediction = 0;
		for (int X = startX; X <= endX; X++) {
			for (int Y = startY; Y <= endY; Y++) {
				prediction += spacialHashMap[{X, Y}].size();
			}
		}

		#pragma warning(supress: 26451)
		results.reserve(prediction + 1);
		for(int X = startX; X <= endX; X++) {
			for(int Y = startY; Y <= endY; Y++) {
				for (auto& elem : spacialHashMap[{X, Y}]) {
					// This may generate duplicates
					// If duplicates shouldn't happen, make sure to remove them in destination method
					results.push_back(elem);
				}
			}
		}
		return results;
	}

	void GameMaster::Stop() {
		gameRunning = false;
	}

	bool GameMaster::IsGameRunning() {
		return gameRunning;
	}

	void GameMaster::UltimateMegaInitOfDestiny() {
		// Initializes the assets and stuff

		// Textures
		Assets.LoadTexture("Checks", "Untitled.png");
		Assets.LoadTexture("Test", "Untitled2.png");
		Assets.LoadTexture("Char", "Char.png");
		Assets.LoadTexture("Test2", "Untitled3.png");
		Assets.LoadTexture("Level", "Level.png");
		Assets.LoadTexture("CharCircle", "CharCircle.png");
		Assets.LoadTexture("Target", "target.png");

		// Fonts
		Assets.LoadSpriteFont("Huge", "Fonts/CourierNewHuge_0.png", "Fonts/CourierNewHuge.fnt");
		Assets.LoadSpriteFont("Big", "Fonts/CourierNewBig_0.png", "Fonts/CourierNewBig.fnt");

		// Music
		Assets.LoadMusic("YourMom", "digi.ogg");

		// Sound
		Assets.LoadSound("Mooz", "Mooz.ogg");

		// Settings
		Graphics.SetDisplayMode(Graphics.VideoModes.at("1920.1080.w"));
		Input.SetMouseGrab(true);

		// Animation info
		AddAnimation("BoxThing", Game::Animation("Test", {}));
		SetAnimationInfo("BoxThing", { 20, 4, 1, Game::AnimatedSprite::LoopMode::NormalLoop });

		AddAnimation("BoxThing2", Game::Animation("Test2", {}));
		SetAnimationInfo("BoxThing2", { 10, 4, 2, Game::AnimatedSprite::LoopMode::PlayOnce });

		AddAnimation("PlayerIdle", Game::Animation("Char", {}));
		SetAnimationInfo("PlayerIdle", { 1337, 1, 1, Game::AnimatedSprite::LoopMode::Static });
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

	void GameMaster::AddCollider(Collider* collider) {
		if (collider != nullptr) {
			colliderLibrary.insert(collider);
		}
	}

	void GameMaster::RemoveCollider(Collider* collider) {
		colliderLibrary.erase(collider);
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

			auto candidates = GetCollisionCandidates(startX, startY, endX, endY);

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
					if (HurtA) {
						if (EntityA != nullptr && EntityA->GetAI() != nullptr) EntityA->GetAI()->OnHitByAttack(EntityB, alpha->GetCombatDamage());
						if (EntityB != nullptr && EntityB->GetAI() != nullptr) EntityB->GetAI()->OnAttackHit();

						//std::cout << "HurtA ";
					}
					if (HurtB) {
						if (EntityB != nullptr && EntityA->GetAI() != nullptr) EntityB->GetAI()->OnHitByAttack(EntityA, beta->GetCombatDamage());
						if (EntityA != nullptr && EntityB->GetAI() != nullptr) EntityA->GetAI()->OnAttackHit();
						//std::cout << "HurtB ";
					}

					//std::cout << "COL" << alpha->GetCombatDamage() << " " << beta->GetCombatDamage() << endl;
				}
				collidedPairs.insert({ alpha, beta });
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