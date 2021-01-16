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
#include "ChaserAI.h"
#include "GunTurretAI.h"
#include "RifleAmmoPack.h"
#include "ConfigHandler.h"

namespace Game {
	GameMaster::GameMaster() :
		entityID(1),
		effectID(1),
		gameRunning(true),
		difficulty(DifficultyLevel::Easy)
	{
	}

	GameMaster::~GameMaster() {
		// Cleanup - probably not required?
		while (entityMasterList.size() > 0) {
			entityMasterList.erase(entityMasterList.begin());
		}
		while (effectMasterList.size() > 0) {
			effectMasterList.erase(effectMasterList.begin());
		}
	}

	void GameMaster::AddThePlayer() {
		if (GetThePlayer() != nullptr) {
			return;
		}
		entityMasterList[maxAutoEntityID + SpecialEntities::Player].reset(new Actor(new PlayerPseudoAI()));
		Game::Actor& player = *(Actor*)entityMasterList[maxAutoEntityID + SpecialEntities::Player].get();
		Game::PlayerPseudoAI& playerLogic = *(PlayerPseudoAI*)player.GetAI();
		player.GetComponent().AddAnimation("Player_PistolIdle");
		player.GetComponent().SetDefaultAnimation("Player_PistolIdle");
		player.GetComponent().SwitchAnimation("Player_PistolIdle");

		player.GetComponent().AddAnimation("Player_RifleIdle");
		player.GetComponent().AddAnimation("Player_PistolShoot");
		player.GetComponent().AddAnimation("Player_PistolReload");
		player.GetComponent().AddAnimation("Player_RifleShoot");
		player.GetComponent().AddAnimation("Player_RifleReload");
		player.GetComponent().AddAnimation("Player_Dead");
		player.GetComponent().AddAnimation("Player_ShieldUp");
		player.GetComponent().AddAnimation("Player_ShieldDown");
		player.GetComponent().AddAnimation("Player_ShieldWalk");
		player.GetComponent().AddAnimation("Player_AxeSwing");
		player.GetComponent().AddAnimation("Player_TakeOutRifle");
		player.GetComponent().AddAnimation("Player_TakeOutPistol");
		player.GetComponent().AddAnimation("Player_PutAwayRifle");
		player.GetComponent().AddAnimation("Player_PutAwayPistol");

		player.GetCollider().SetCombatLayer(Collider::CombatLayer::Players);
		player.GetCollider().SetRadius(40.0);

		player.SetType(EntityType::Player);

		auto& stats = player.GetStatsReference();
		stats.maxHealth = stats.health = 100.0;
		stats.shieldHealth = stats.maxShieldHealth = 60.0;
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

	uint64_t GameMaster::AddEntity(EntityType type, Vector2 worldPos) {
		uint64_t ID;
		switch (type) {
		case EntityType::Fighter: {
			ID = NextEntityID();
			entityMasterList[ID].reset(new Actor(new FighterAI()));
			Game::Actor& fighter = *(Actor*)entityMasterList[ID].get();
			fighter.GetComponent().AddAnimation("Player_PistolIdle");
			fighter.GetComponent().AddAnimation("Player_PistolShoot");
			fighter.GetComponent().AddAnimation("Player_Dead");
			fighter.GetComponent().SetDefaultAnimation("Player_PistolIdle");
			fighter.GetComponent().SwitchAnimation("Player_PistolIdle");
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
			bullet.GetComponent().AddAnimation("Bullet_Travel");
			bullet.GetComponent().SetDefaultAnimation("Bullet_Travel");
			bullet.GetComponent().SwitchAnimation("Bullet_Travel");
			bullet.GetCollider().SetEntityDestructionSignalling(true);

			bullet.SetType(EntityType::FighterBulletProjectile);

			bullet.GetTransform().position = worldPos;
		} break;
		case EntityType::Knight: {
			ID = NextEntityID();
			entityMasterList[ID].reset(new Actor(new KnightAI()));
			Game::Actor& knight = *(Actor*)entityMasterList[ID].get();
			knight.GetComponent().AddAnimation("Knight_Walk");
			knight.GetComponent().SetDefaultAnimation("Knight_Walk");
			knight.GetComponent().SwitchAnimation("Knight_Walk");
			knight.GetComponent().AddAnimation("Knight_Swing");
			knight.GetComponent().AddAnimation("Knight_Dead");
			knight.GetCollider().SetCombatLayer(Collider::CombatLayer::Enemies);

			knight.SetType(EntityType::Knight);

			auto& stats = knight.GetStatsReference();
			double HP = 100.0;
			HP += GetDifficulty() * 30.0;
			stats.maxHealth = stats.health = HP;

			knight.GetTransform().position = worldPos;
		} break;
		case EntityType::Chaser: {
			ID = NextEntityID();
			entityMasterList[ID].reset(new Actor(new ChaserAI()));
			Game::Actor& axeman = *(Actor*)entityMasterList[ID].get();
			axeman.GetComponent().AddAnimation("Berserker_Walk");
			axeman.GetComponent().SetDefaultAnimation("Berserker_Walk");
			axeman.GetComponent().SwitchAnimation("Berserker_Walk");
			axeman.GetComponent().AddAnimation("Berserker_AxeSwing");
			axeman.GetComponent().AddAnimation("Berserker_Dead");
			axeman.GetCollider().SetCombatLayer(Collider::CombatLayer::Enemies);

			axeman.SetType(EntityType::Chaser);

			auto& stats = axeman.GetStatsReference();
			double HP = 130.0;
			HP += GetDifficulty() * 40.0;
			stats.maxHealth = stats.health = HP;

			axeman.GetTransform().position = worldPos;
		} break;
		case EntityType::GunTurret: {
			ID = NextEntityID();
			entityMasterList[ID].reset(new Actor(new GunTurretAI()));
			Game::Actor& turret = *(Actor*)entityMasterList[ID].get();
			turret.GetComponent().AddAnimation("GunTurret_Idle");
			turret.GetComponent().SetDefaultAnimation("GunTurret_Idle");
			turret.GetComponent().SwitchAnimation("GunTurret_Idle");
			turret.GetComponent().AddAnimation("GunTurret_Shoot");
			turret.GetComponent().AddAnimation("GunTurret_Broken");
			turret.GetCollider().SetCombatLayer(Collider::CombatLayer::Enemies);
			turret.GetCollider().SetColliderType(Collider::ColliderType::CombatStatic);
			turret.GetCollider().SetRadius(35.0);

			turret.SetType(EntityType::GunTurret);

			auto& stats = turret.GetStatsReference();
			double HP = 195;
			HP += GetDifficulty() * 70.0;
			stats.maxHealth = stats.health = HP;

			turret.GetTransform().position = worldPos;
		} break;
		case EntityType::RifleAmmoPackThing: {
			ID = NextEntityID();
			entityMasterList[ID].reset(new RifleAmmoPack());
			Game::Actor& turret = *(Actor*)entityMasterList[ID].get();
			turret.GetComponent().AddAnimation("RifleAmmoPack");
			turret.GetComponent().SwitchAnimation("RifleAmmoPack");
			turret.GetComponent().SetDefaultAnimation("RifleAmmoPack");

			turret.SetType(EntityType::RifleAmmoPackThing);
			turret.GetTransform().position = worldPos;
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
			auto type = entity.second->GetType();
			if (type == EntityType::Fighter || type == EntityType::Knight || type == EntityType::Chaser) {
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
					threat += 25;
					break;
				case EntityType::Knight:
					threat += 40;
					break;
				case EntityType::Chaser:
					threat += 75;
					break;
				case EntityType::GunTurret:
					threat += 20;
					break;
				}
			}
		}
		return threat;
	}

	void GameMaster::CreateDefaultTracerEffect(const Vector2& start, const Vector2& end) {
		uint64_t ID = NextEffectID();
		effectMasterList[ID].reset(new Tracer(start, end));
		auto& tracer = *((Tracer*)effectMasterList[ID].get());
		tracer.SetTimeLeft(30);
		tracer.SetFadeTime(30);
		tracer.SetBodyTexture("DefaultTracer");
		tracer.RegisterToGame();
		tracer.SetLayer(GraphicsEngine::CommonLayers::BelowDefaultHeight);
		tracer.SetAlpha(127);
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

	uint64_t GameMaster::NextEffectID() {
		uint64_t returnID = effectID++;
		effectID = (effectID >= maxAutoEffectID ? 1 : effectID);
		return returnID;
	}

	void GameMaster::Update(bool skipGraphicsFrame = false) {
		if (!gameRunning) {
			return;
		}

		Input.Update();

		if (Input.WasQuitCalled()) {
			Stop();
			return;
		}

		for (auto& pair : entityMasterList) {
			pair.second->Update();
		}

		BuildSpacialHashMap();
		ResolveMovementCollisions();
		ResolveCombatCollisions();

		for (auto& pair : effectMasterList) {
			pair.second->Update();
		}
		
		// Destroy marked entities

		vector<uint64_t> entitiesToDelete;
		for (auto& pair : entityMasterList) {
			if (pair.second->IsDestructionSignalled()) {
				entitiesToDelete.push_back(pair.first);
			}
		}

		for (auto& entity : entitiesToDelete) {
			entityMasterList.erase(entity);
		}

		// Destroy expired effects

		vector<uint64_t> effectsToDelete;
		for (auto& pair : effectMasterList) {
			if (pair.second->GetTimeLeft() <= 0) {
				effectsToDelete.push_back(pair.first);
			}
		}

		for (auto& effect : effectsToDelete) {
			effectMasterList.erase(effect);
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
		string configPath = "config.cfg";
		if (!ConfigHandler::ReadConfig(configPath)) {
			ConfigHandler::CreateDefaultConfig(configPath);
			ConfigHandler::ReadConfig(configPath);
		}

		// Initializes the assets and stuff

		// Textures
		const string texturePath = "Textures/";
		string assetFolder = "";

		// Level
		assetFolder = texturePath + "Level/";
		Assets.LoadTexture("WallTile", assetFolder + "WallTile.png");
		Assets.LoadTexture("LevelFloor", assetFolder + "LevelFloor.png");
		Assets.LoadTexture("SmallPillar", assetFolder + "SmallPillar.png");
		Assets.LoadTexture("LargePillar", assetFolder + "LargePillar.png");

		// Player Related
		assetFolder = texturePath + "Player/";
		Assets.LoadTexture("Player_PistolIdle", assetFolder + "PistolIdle.png");
		Assets.LoadTexture("Player_RifleIdle", assetFolder + "RifleIdle.png");
		Assets.LoadTexture("Player_Dead", assetFolder + "Dead.png");
		Assets.LoadTexture("Player_PistolShoot", assetFolder + "PistolShoot.png");
		Assets.LoadTexture("Player_PistolReload", assetFolder + "PistolReload.png");
		Assets.LoadTexture("Player_RifleShoot", assetFolder + "RifleShoot.png");
		Assets.LoadTexture("Player_RifleReload", assetFolder + "RifleReload.png");
		Assets.LoadTexture("Player_ShieldUp", assetFolder + "ShieldUp.png");
		Assets.LoadTexture("Player_ShieldDown", assetFolder + "ShieldDown.png");
		Assets.LoadTexture("Player_ShieldWalk", assetFolder + "ShieldWalk.png");
		Assets.LoadTexture("Player_AxeSwing", assetFolder + "AxeSwing.png");
		Assets.LoadTexture("Player_TakeOutRifle", assetFolder + "TakeOutRifle.png");
		Assets.LoadTexture("Player_TakeOutPistol", assetFolder + "TakeOutPistol.png");
		Assets.LoadTexture("Player_PutAwayRifle", assetFolder + "PutAwayRifle.png");
		Assets.LoadTexture("Player_PutAwayPistol", assetFolder + "PutAwayPistol.png");

		// GUI
		assetFolder = texturePath + "GUI/";
		Assets.LoadTexture("Target", assetFolder + "Target.png");
		Assets.LoadTexture("Button", assetFolder + "Button.png");
		Assets.LoadTexture("LowHP", assetFolder + "LowHP.png");
		Assets.LoadTexture("ValueButton", assetFolder + "ValueButton.png");
		
		// Knight Enemy
		assetFolder = texturePath + "Knight/";
		Assets.LoadTexture("Knight_Walk", assetFolder + "KnightWalk.png");
		Assets.LoadTexture("Knight_Swing", assetFolder + "KnightSwing.png");
		Assets.LoadTexture("Knight_Dead", assetFolder + "KnightDead.png");
		
		// Berserker Enemy
		assetFolder = texturePath + "Berserker/";
		Assets.LoadTexture("Berserker_AxeSwing", assetFolder + "ChaserAxeSwing.png");
		Assets.LoadTexture("Berserker_Walk", assetFolder + "ChaserIdleNew.png");
		Assets.LoadTexture("Berserker_Dead", assetFolder + "ChaserDead.png");
		
		// Turret Enemy
		assetFolder = texturePath + "Turret/";
		Assets.LoadTexture("GunTurret_Idle", assetFolder + "GunTurretIdle.png");
		Assets.LoadTexture("GunTurret_Broken", assetFolder + "GunTurretBroken.png");
		Assets.LoadTexture("GunTurret_Shoot", assetFolder + "GunTurretShoot.png");

		// Misc
		assetFolder = texturePath + "Misc/";
		Assets.LoadTexture("RifleAmmoPack", assetFolder + "Ammo.png");
		Assets.LoadTexture("Bullet", assetFolder + "Bullet.png");

		// Shadow - Boss
		assetFolder = texturePath + "Shadow/";
		Assets.LoadTexture("Shadow_PistolIdle", assetFolder + "PistolIdle.png");
		Assets.LoadTexture("Shadow_AxeSwing", assetFolder + "AxeSwing.png");

		// Effects
		const string effectPath = texturePath + "Effects/";
		Assets.LoadTexture("DefaultTracer", effectPath + "DefaultTracer.png");

		// Fonts
		const string fontPath = "Fonts/";
		Assets.LoadSpriteFont("Huge", fontPath + "CourierNewHuge_0.png", fontPath + "CourierNewHuge.fnt");
		Assets.LoadSpriteFont("Big", fontPath + "CourierNewBig_0.png", fontPath + "CourierNewBig.fnt");

		

		const string audioPath = "Audio/";
		// Music
		Assets.LoadMusic("DigitalGhost", audioPath + "digi.ogg");
		Assets.LoadMusicSections("DigitalGhost", {
			{"Loop", {(1 * 60 + 48) * 1000 + 199, (3 * 60 + 1) * 1000 + 566 }}
			});

		Assets.LoadMusic("Menu", audioPath + "menu.ogg");

		// Sound
		Assets.LoadSound("PlayerShoot", audioPath + "shot.ogg");
		Assets.LoadSound("PlayerReload1", audioPath + "magout.ogg");
		Assets.LoadSound("PlayerReload2", audioPath + "magin.ogg");
		Assets.LoadSound("PlayerReload3", audioPath + "receiverpull.ogg");
		Assets.LoadSound("GunClick", audioPath + "gunclick.ogg");
		Assets.LoadSound("SwordSwing", audioPath + "swordswing.ogg");
		Assets.LoadSound("ShieldImpact", audioPath + "shieldimpact.ogg");
		Assets.LoadSound("PerfectGuard", audioPath + "perfectguard.ogg");
		Assets.LoadSound("ShieldBreak", audioPath + "shieldbreak.ogg");
		Assets.LoadSound("ButtonClick", audioPath + "buttonclick.ogg");
		Assets.LoadSound("Death", audioPath + "death.ogg");
		Assets.LoadSound("PlayerDeath", audioPath + "playerdeath.ogg");
		Assets.LoadSound("PlayerHurt", audioPath + "playerhurt.ogg");
		Assets.LoadSound("HurtBeta", audioPath + "hurt2.ogg");
		Assets.LoadSound("NextWave", audioPath + "nextwave.ogg");
		Assets.LoadSound("Heartbeat", audioPath + "heartbeat.ogg");
		Assets.LoadSound("Boost", audioPath + "boost.ogg");
		Assets.LoadSound("ZerkHurt", audioPath + "zerkhurt.ogg");
		Assets.LoadSound("TurretHurt", audioPath + "turrethurt.ogg");
		Assets.LoadSound("TurretBreak", audioPath + "turretbreak.ogg");
		Assets.LoadSound("TurretDown", audioPath + "turretdown.ogg");
		Assets.LoadSound("TurretCharge", audioPath + "turretcharge.ogg");

		// Settings
		auto res = ConfigHandler::GetConfigResolution();
		auto mode = ConfigHandler::GetConfigFullscreen();
		Graphics.SetDisplayMode({ res.first, res.second, mode });
		Audio.SetUserMusicVolume(ConfigHandler::GetMusicVolume());
		Audio.SetUserSoundVolume(ConfigHandler::GetSoundVolume());

		Input.SetMouseGrab(true);

		// Animation info
		using Animation = Game::Animation;
		using LoopMode = Game::AnimatedSprite::LoopMode;
		using AnimationCriteria = Animation::AnimationCriteria;
		using AnimationInstruction = Animation::AnimationInstruction;

		// Player
		AddAnimation("Player_PistolIdle", Animation("Player_PistolIdle", {}));
		SetAnimationInfo("Player_PistolIdle", { 1337, 1, 1, LoopMode::Static });
		SetAnimationCenter("Player_PistolIdle", Vector2(48, 67));

		AddAnimation("Player_RifleIdle", Animation("Player_RifleIdle", {}));
		SetAnimationInfo("Player_RifleIdle", { 1337, 1, 1, LoopMode::Static });
		SetAnimationCenter("Player_RifleIdle", Vector2(48, 67));

		AddAnimation("Player_Dead", Animation("Player_Dead", {}));
		SetAnimationInfo("Player_Dead", { 1337, 1, 1, LoopMode::Static });
		SetAnimationCenter("Player_Dead", Vector2(75, 100));

		AddAnimation("Player_PistolShoot", Animation("Player_PistolShoot", { 
			{ {AnimationCriteria::TriggerAtStart, ""}, {AnimationInstruction::PlaySound, "PlayerShoot"} } 
			}));
		SetAnimationInfo("Player_PistolShoot", { 4, 4, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Player_PistolShoot", Vector2(48, 67));

		AddAnimation("Player_PistolReload", Animation("Player_PistolReload", {
			{ {AnimationCriteria::TriggerAtFrameX, "3"}, {AnimationInstruction::PlaySound, "PlayerReload1"} },
			{ {AnimationCriteria::TriggerAtFrameX, "8"}, {AnimationInstruction::PlaySound, "PlayerReload2"} },
			{ {AnimationCriteria::TriggerAtFrameX, "10"}, {AnimationInstruction::PlaySound, "PlayerReload3"} }
			}));
		SetAnimationInfo("Player_PistolReload", { 8, 12, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Player_PistolReload", Vector2(48, 67));

		AddAnimation("Player_RifleShoot", Animation("Player_RifleShoot", { 
			{ {AnimationCriteria::TriggerAtStart, ""}, {AnimationInstruction::PlaySound, "PlayerShoot"} } 
			}));
		SetAnimationInfo("Player_RifleShoot", { 2, 4, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Player_RifleShoot", Vector2(48, 87));

		AddAnimation("Player_RifleReload", Animation("Player_RifleReload", {
			{ {AnimationCriteria::TriggerAtFrameX, "5"}, {AnimationInstruction::PlaySound, "PlayerReload1"} },
			{ {AnimationCriteria::TriggerAtFrameX, "16"}, {AnimationInstruction::PlaySound, "PlayerReload2"} },
			{ {AnimationCriteria::TriggerAtFrameX, "19"}, {AnimationInstruction::PlaySound, "PlayerReload3"} }
			}));
		SetAnimationInfo("Player_RifleReload", { 7, 21, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Player_RifleReload", Vector2(48, 67));

		AddAnimation("Player_ShieldUp", Animation("Player_ShieldUp", {
			{ {AnimationCriteria::TriggerAtEnd, ""}, {AnimationInstruction::SwitchAnimation, "Player_ShieldWalk"} }
			}));
		SetAnimationInfo("Player_ShieldUp", { 3, 4, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Player_ShieldUp", Vector2(73, 66));

		AddAnimation("Player_ShieldDown", Animation("Player_ShieldDown", {}));
		SetAnimationInfo("Player_ShieldDown", { 3, 4, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Player_ShieldDown", Vector2(73, 66));

		AddAnimation("Player_ShieldWalk", Animation("Player_ShieldWalk", {}));
		SetAnimationInfo("Player_ShieldWalk", { 12, 4, 1, LoopMode::NormalLoop });
		SetAnimationCenter("Player_ShieldWalk", Vector2(73, 66));

		AddAnimation("Player_AxeSwing", Animation("Player_AxeSwing", {
			{ {AnimationCriteria::TriggerAtFrameX, "4"}, {AnimationInstruction::PlaySound, "SwordSwing"} }
			}));
		SetAnimationInfo("Player_AxeSwing", { 5, 12, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Player_AxeSwing", Vector2(73, 116));

		AddAnimation("Player_TakeOutRifle", Animation("Player_TakeOutRifle", {
			{ {AnimationCriteria::TriggerAtStart, ""}, {AnimationInstruction::PlaySound, "PlayerReload1"} }
			}));
		SetAnimationInfo("Player_TakeOutRifle", { 4, 5, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Player_TakeOutRifle", Vector2(48, 67));

		AddAnimation("Player_TakeOutPistol", Animation("Player_TakeOutPistol", {
			{ {AnimationCriteria::TriggerAtStart, ""}, {AnimationInstruction::PlaySound, "PlayerReload1"} }
			}));
		SetAnimationInfo("Player_TakeOutPistol", { 4, 5, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Player_TakeOutPistol", Vector2(48, 67));

		AddAnimation("Player_PutAwayRifle", Animation("Player_PutAwayRifle", {}));
		SetAnimationInfo("Player_PutAwayRifle", { 4, 5, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Player_PutAwayRifle", Vector2(48, 67));

		AddAnimation("Player_PutAwayPistol", Animation("Player_PutAwayPistol", {}));
		SetAnimationInfo("Player_PutAwayPistol", { 4, 5, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Player_PutAwayPistol", Vector2(48, 67));

		// Knight Enemy
		AddAnimation("Knight_Walk", Animation("Knight_Walk", {}));
		SetAnimationInfo("Knight_Walk", { 12, 4, 1, LoopMode::NormalLoop });
		SetAnimationCenter("Knight_Walk", Vector2(73, 66));

		AddAnimation("Knight_Swing", Animation("Knight_Swing", {
			{ {AnimationCriteria::TriggerAtFrameX, "5"}, {AnimationInstruction::PlaySound, "SwordSwing"} }
			}));
		SetAnimationInfo("Knight_Swing", { 6, 12, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Knight_Swing", Vector2(73, 116));

		AddAnimation("Knight_Dead", Animation("Knight_Dead", {}));
		SetAnimationInfo("Knight_Dead", { 1337, 1, 1, LoopMode::Static });
		SetAnimationCenter("Knight_Dead", Vector2(75, 100));

		// Berserker Enemy

		AddAnimation("Berserker_AxeSwing", Animation("Berserker_AxeSwing", {
			{ {AnimationCriteria::TriggerAtFrameX, "4"}, {AnimationInstruction::PlaySound, "SwordSwing"} }
			}));
		SetAnimationInfo("Berserker_AxeSwing", { 3, 12, 1, LoopMode::PlayOnce });
		SetAnimationCenter("Berserker_AxeSwing", Vector2(73, 116));

		AddAnimation("Berserker_Walk", Animation("Berserker_Walk", {}));
		SetAnimationInfo("Berserker_Walk", { 12, 4, 1, LoopMode::NormalLoop });
		SetAnimationCenter("Berserker_Walk", Vector2(73, 116));

		AddAnimation("Berserker_Dead", Animation("Berserker_Dead", {}));
		SetAnimationInfo("Berserker_Dead", { 1337, 1, 1, LoopMode::Static });
		SetAnimationCenter("Berserker_Dead", Vector2(75, 100));

		// Turret Enemy
		AddAnimation("GunTurret_Idle", Animation("GunTurret_Idle", {}));
		SetAnimationInfo("GunTurret_Idle", { 1337, 1, 1, LoopMode::Static });
		SetAnimationCenter("GunTurret_Idle", Vector2(50, 64));

		AddAnimation("GunTurret_Shoot", Animation("GunTurret_Shoot", { 
			{ {AnimationCriteria::TriggerAtStart, ""}, {AnimationInstruction::PlaySound, "PlayerShoot"} } 
			}));
		SetAnimationInfo("GunTurret_Shoot", { 2, 8, 1, LoopMode::PlayOnce });
		SetAnimationCenter("GunTurret_Shoot", Vector2(50, 64));

		AddAnimation("GunTurret_Broken", Animation("GunTurret_Broken", {}));
		SetAnimationInfo("GunTurret_Broken", { 1337, 1, 1, LoopMode::Static });
		SetAnimationCenter("GunTurret_Broken", Vector2(50, 64));

		// Misc
		AddAnimation("Bullet_Travel", Animation("Bullet", {}));
		SetAnimationInfo("Bullet_Travel", { 1337, 1, 1, LoopMode::Static });
		SetAnimationCenter("Bullet_Travel", Vector2(4, 6));

		AddAnimation("RifleAmmoPack", Animation("RifleAmmoPack", {}));
		SetAnimationInfo("RifleAmmoPack", { 1337, 1, 1, LoopMode::Static });
		SetAnimationCenter("RifleAmmoPack", Vector2(20, 15));
	}

	void GameMaster::ArmageddonExitProcedures() {
		// Save config
		string configPath = "config.cfg";
		ConfigHandler::SaveConfig(configPath);
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
				if ((alpha->GetColliderType() == CType::Static || alpha->GetColliderType() == CType::CombatStatic) && (beta->GetColliderType() == CType::Static || beta->GetColliderType() == CType::CombatStatic)) {
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

				if (alpha->GetColliderType() == CType::Static || alpha->GetColliderType() == CType::CombatStatic) {
					APushStr = 0.0;
					BPushStr = 1.0;
				}
				else if (beta->GetColliderType() == CType::Static || beta->GetColliderType() == CType::CombatStatic) {
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
			if (!(alpha->GetColliderType() == CType::Combat || alpha->GetColliderType() == CType::CombatDynamic || alpha->GetColliderType() == CType::CombatStatic)) {
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
				if (!(beta->GetColliderType() == CType::Combat || beta->GetColliderType() == CType::CombatDynamic || beta->GetColliderType() == CType::CombatStatic)) {
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
					
					if (alpha->GetCollisionOptions().find(Collider::CollisionOptions::DoNotHitRememberedEntities) != alpha->GetCollisionOptions().end()) {
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

					if (beta->GetCollisionOptions().find(Collider::CollisionOptions::DoNotHitRememberedEntities) != beta->GetCollisionOptions().end()) {
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
			if (!(alpha->GetColliderType() == CType::Combat || alpha->GetColliderType() == CType::CombatDynamic || alpha->GetColliderType() == CType::CombatStatic)) {
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
				if (beta->GetColliderType() != Collider::ColliderType::Static && beta->GetColliderType() != CType::CombatStatic) {
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