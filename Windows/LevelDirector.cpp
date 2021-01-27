#include "PCHeader.h"
#include "LevelDirector.h"
#include "Globals.h"
#include "Actor.h"
#include "GunTurretAI.h"
#include "LogHandler.h"
#include "ShadowAI.h"
#include "RifleAmmoPack.h"
#include "Medkit.h"

namespace Game {
    LevelDirector::LevelDirector() :
        levelBoxes({
            BoxCollider(Game::Vector2(0, 0), 2400, 100, Game::Collider::ColliderType::Static),
            BoxCollider(Game::Vector2(0, 0), 100, 2000, Game::Collider::ColliderType::Static),
            BoxCollider(Game::Vector2(0, 1900), 2400, 100, Game::Collider::ColliderType::Static),
            BoxCollider(Game::Vector2(2300, 0), 100, 2000, Game::Collider::ColliderType::Static)
            }),
        levelSpheres({
                // Small pillars
            SphereCollider(Game::Vector2(840, 500), 50, Game::Collider::ColliderType::Static),
            SphereCollider(Game::Vector2(1600, 500), 50, Game::Collider::ColliderType::Static),
            SphereCollider(Game::Vector2(840, 1500), 50, Game::Collider::ColliderType::Static),
            SphereCollider(Game::Vector2(1600, 1500), 50, Game::Collider::ColliderType::Static),
            SphereCollider(Game::Vector2(1835, 970), 50, Game::Collider::ColliderType::Static),
            SphereCollider(Game::Vector2(520, 970), 50, Game::Collider::ColliderType::Static),
                //Big pillar
            SphereCollider(Game::Vector2(1200, 970), 100, Game::Collider::ColliderType::Static),
            }),
        dynamicSpawnPoints({
            {Vector2(150, 150), 135.0},
            {Vector2(400, 150), 160.0},
            {Vector2(150, 700), 80.0},
            {Vector2(2250, 150), 225.0},
            {Vector2(2250, 1850), 315.0},
            {Vector2(2250, 1650), 315.0},
            {Vector2(150, 1850), 45.0}
        }),
        staticSpawnPoints({
            {Vector2(500, 1500), 45.0},
            {Vector2(2000, 1500), 315.0},
            {Vector2(500, 500), 135.0},
            {Vector2(2000, 500), 225.0},
        }),
        audioStartDelay(60),
        spawnCredits(0),
        currentWave(0),
        nextSpawns(0),
        counter(0),
        resetCounter(0),
        currentSpawnCount({ 0, 0, 0 }),
        didTurretSpawns(false),
        bossSequence(false),
        bossSpawned(false)
    {
        auto& ProjectSilver = Globals::Game();

        ProjectSilver.Audio.StopMusic();

        // Position the Player in the level
        ProjectSilver.AddThePlayer();
        ProjectSilver.GetThePlayer()->GetTransform().position = Game::Vector2(800, 200);

        // Level Colliders
        for (auto& box : levelBoxes) {
            box.RegisterToGame();
        }
        for (auto& sphere : levelSpheres) {
            sphere.RegisterToGame();
        }

        // Level Sprites

        // This texture is big, but I want the floor to not be patternized
        Game::Sprite& floorSprite = levelSprites.emplace_back();
        floorSprite.SetTexture("LevelFloor");
        floorSprite.SetPosition({ 0, 0 });
        floorSprite.SetCenter({ 0, 0 });
        floorSprite.SetLayer(Game::GraphicsEngine::CommonLayers::Floor);
        floorSprite.RegisterToGame();

        // Wall Sprites - made using repeated tiles

        // Top and bottom walls
        for (int x = 150; x <= 2250; x += 100) {
            for (int y = 50; y <= 1950; y += 1900) {
                Game::Sprite& wallTile = levelSprites.emplace_back();
                wallTile.SetTexture("WallTile");
                wallTile.SetPosition(Vector2(x, y));
                wallTile.SetCenter(Vector2(50, 50));
                wallTile.SetLayer(Game::GraphicsEngine::CommonLayers::DefaultHeight);
                wallTile.RegisterToGame();
            }
        }

        // Left and right walls
        for (int y = 50; y <= 1950; y += 100) {
            for (int x = 50; x <= 2350; x += 2300) {
                Game::Sprite& wallTile = levelSprites.emplace_back();
                wallTile.SetTexture("WallTile");
                wallTile.SetDirection(90.0);
                wallTile.SetPosition(Vector2(x, y));
                wallTile.SetCenter(Vector2(50, 50));
                wallTile.SetLayer(Game::GraphicsEngine::CommonLayers::DefaultHeight);
                wallTile.RegisterToGame();
            }
        }

        // Pillars

        for (auto& sphere : levelSpheres) {
            Game::Sprite& pillar = levelSprites.emplace_back();
            if(sphere.GetRadius() == 50.0) {
                pillar.SetTexture("SmallPillar");
                pillar.SetCenter(Vector2(52, 52));
            }
            else {
                pillar.SetTexture("LargePillar");
                pillar.SetCenter(Vector2(102, 102));
            }
            pillar.SetPosition(sphere.GetTransform()->position);
            pillar.SetLayer(Game::GraphicsEngine::CommonLayers::DefaultHeight);
            pillar.RegisterToGame();
        }

        // GUI Objects
        waveText.SetFont("Big");
        waveText.SetRelativeToCamera(false);
        waveText.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
        waveText.SetPosition(Vector2(1860, 1020));
        waveText.RegisterToGame();
        waveText.SetRenderType(BasicText::TextRenderType::ContinuousRight);
        waveText.SetText("They're coming...");

        difficultyText.SetFont("Big");
        difficultyText.SetRelativeToCamera(false);
        difficultyText.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
        difficultyText.SetRenderType(BasicText::TextRenderType::ContinuousRight);
        difficultyText.SetPosition(Vector2(1860, 970));
        difficultyText.RegisterToGame();

        switch (Globals::Difficulty()) {
        case 0: {
            difficultyText.SetText("Easy");
            difficultyText.SetColor(Color::Aqua);
        } break;
        case 1: {
            difficultyText.SetText("Normal");
            difficultyText.SetColor(Color::White);
        } break;
        case 2: {
            difficultyText.SetText("Hard");
            difficultyText.SetColor(Color::Red);
        } break;
        }
    }

    LevelDirector::~LevelDirector() 
    {
        // Sprites and colliders should be unregistered via their destructor code
        auto& ProjectSilver = Globals::Game();
        ProjectSilver.RemoveThePlayer();
        ProjectSilver.RemoveTheShadow();
        ProjectSilver.RemoveNonSpecialEntities();
        ProjectSilver.ClearEffects(); // Removes bullet trails and whatnot
        Game::LogHandler::Log("Exited Level", Game::LogHandler::MessageType::Info);
    }

    void LevelDirector::Update() {
        // 'Should' be safe
        Actor* player = (Actor*)Globals::ThePlayer();
        Actor* shadow = (Actor*)Globals::Game().GetTheShadow();
        if (player != nullptr) {
            bool deadPlayer = player->GetStatsReference().isDead;
            if (deadPlayer || bossSpawned && shadow != nullptr && shadow->GetStatsReference().isDead) {
                bool deadBoss = shadow->GetStatsReference().isDead;
                resetCounter++;
                if (deadPlayer) {
                    if (resetCounter == 180) {
                        toBeDestroyed = true;
                        Globals::Game().InitMenu();
                    }
                }
                else if (deadBoss) {
                    player->GetStatsReference().invulnerable = true;
                    Globals::Graphics().SetGameWinFadeout(resetCounter / 2.7);
                    if (resetCounter == 300) {
                        toBeDestroyed = true;
                        Globals::Game().InitMenu();
                    }
                }
                return;
            }
        }

        if (audioStartDelay > 0) {
            audioStartDelay--;
            if (audioStartDelay == 0) {
                if (bossSequence) {
                    Globals::Audio().PlayMusic("Shadows");
                    Globals::Audio().SetLoopSection("Loop");
                }
                else {
                    Globals::Audio().PlayMusic("DigitalGhost");
                    Globals::Audio().SetLoopSection("Loop");
                }
            }
        }

        if (bossSequence) {
            ressuplyCounter++;
            if (ressuplyCounter >= 400 + 200 * Globals::Difficulty()) {
                for (int i = 0; i < 2; i++) {
                    uint64_t ID = Globals::Game().AddEntity(EntityType::RifleAmmoPackThing, staticSpawnPoints[rand() % staticSpawnPoints.size()].first + Vector2::NormalVector(rand() % 360) * 55);
                    RifleAmmoPack* pack = (RifleAmmoPack*)Globals::Game().GetEntity(ID);
                    if (pack != nullptr) {
                        pack->SetAmmoToGrant(10 - Globals::Difficulty());
                    }
                }
                ressuplyCounter = 0;
                auto& stats = ((Actor*)Globals::ThePlayer())->GetStatsReference();
                if (stats.maxHealth <= 95.0) {
                    uint64_t ID = Globals::Game().AddEntity(EntityType::MedkitThing, staticSpawnPoints[rand() % staticSpawnPoints.size()].first + Vector2::NormalVector(rand() % 360) * 55);
                    Medkit* pack = (Medkit*)Globals::Game().GetEntity(ID);
                    if (pack != nullptr) {
                        pack->SetHealthToGrant(18 - 3 * Globals::Difficulty());
                    }
                    ressuplyCounter -= 570 + 180 * Globals::Difficulty();
                }
            }
        }

        bool lastStand = false;
        if (shadow != nullptr && ((ShadowAI*)((Actor*)shadow)->GetAI()) != nullptr) {
            lastStand = ((ShadowAI*)((Actor*)shadow)->GetAI())->InLastStand();
        }

        if (!didBossTurrets && lastStand) {
            didBossTurrets = true;
            counter = nextSpawns;
            int turretCount = 0;

            set<int> availableStaticSpawnPoints;
            for (int i = 0; i < staticSpawnPoints.size(); i++) {
                availableStaticSpawnPoints.insert(i);
            }

            while (turretCount < 1 + (Globals::Difficulty() > 0 ? 1 : 0)) {
                int spawnPoint = rand() % staticSpawnPoints.size();
                int tries = 50;
                while (availableStaticSpawnPoints.find(spawnPoint) == availableStaticSpawnPoints.end() && tries > 0) {
                    tries--;
                    spawnPoint = rand() % staticSpawnPoints.size();
                }

                uint64_t ID = Globals::Game().AddEntity(EntityType::GunTurret, staticSpawnPoints[spawnPoint].first);
                Entity* entity = Globals::Game().GetEntity(ID);
                entity->GetTransform().direction = staticSpawnPoints[spawnPoint].second;
                entitiesToKill.push_back(ID);

                availableStaticSpawnPoints.erase(spawnPoint);
                turretCount++;
            }

            
        }

        bool outOfCredits = spawnCredits <= 0;

        // Threat Level of 1 means low enemy count Threat Level of 4 should mean you're getting swarmed

        int waveScale = currentWave;
        if (bossSequence) {
            waveScale = 10;
        }

        int activeThreat = Globals::Game().GetActiveThreat() / (35 + Globals::Difficulty() * 5 + waveScale * 8);
        double mercyChance = activeThreat / (2.0 + activeThreat) * 100.0;

        if (mercyChance > 60.0) {
            mercyChance = 60.0;
        }

        if (bossSequence) {
            mercyChance *= 1.2;
            if (lastStand) {
                mercyChance *= 1.2;
            }
        }

        if (rand() % 100 > mercyChance) {
            // Mercy check failed for this frame
            counter++;
        }

        if (didTurretSpawns && counter + 60 <= nextSpawns && Globals::Game().GetAliveEnemyCount() == 0) {
            // Speed up next subwave if all enemies from current subwave are dead
            counter++;
        }

        // Waits until next spawns. If out of credits, advances the counter to current next spawns
        if (counter <= nextSpawns) {
            if (outOfCredits && !bossSequence) {
                counter = nextSpawns;
            }
            return;
        }

        if (outOfCredits && !bossSequence) {
            int enemiesAlive = Globals::Game().GetAliveEnemyCount();
            if (enemiesAlive < 3) {
                // Turrets get disabled, dropping one ammo pack
                KillTrackedEntities();
            }
            if (enemiesAlive > 0) {
                return;
            }

            // Wave beaten
            if (currentWave != 0) {
                waveText.SetText("Wave cleared!");
                Globals::Audio().PlaySound("NextWave");
            }

            // Start Boss sequence on wave 7
            if (!bossSequence && currentWave == 6) {
                Globals::Audio().StopMusic();
                audioStartDelay = 60;
                bossSequence = true;

                // Scope Start
                {
                    uint64_t ID = Globals::Game().AddEntity(EntityType::MedkitThing, Vector2(1200, 1000) + Vector2::NormalVector(rand() % 360) * 255);
                    Medkit* pack = (Medkit*)Globals::Game().GetEntity(ID);
                    if (pack != nullptr) {
                        pack->SetHealthToGrant(200);
                    }
                }
                // Scope end
            }


            currentWave++;
            currentSpawnCount.fill(0);

            // This dictates the current wave's total strength
            spawnCredits = 1000 + (currentWave / 2) * 300;
            
            didTurretSpawns = false;
            counter = 0;
            nextSpawns = 220;

            return;
        }

        

        if (bossSequence) {
            RunBossSpawnLogic();
            waveText.SetText("Last Fight");
        }
        else {
            RunWaveSpawnLogic();
            waveText.SetText("Wave " + std::to_string(currentWave));
        }

        counter = 0;
    }

    void LevelDirector::KillTrackedEntities() {
        if (entitiesToKill.size() > 0) {
            for (auto ID : entitiesToKill) {
                Entity* entity = Globals::Game().GetEntity(ID);
                if (entity != nullptr) {
                    if (entity->GetType() == EntityType::GunTurret) {
                        GunTurretAI* theAI = ((GunTurretAI*)(((Actor*)entity)->GetAI()));
                        theAI->ApplyWaveEndBonus();
                        theAI->OnDeath();
                    }
                }
            }
            entitiesToKill.clear();
        }
    }

    // This function has a lot of variables in play

    void LevelDirector::RunWaveSpawnLogic() {
        // Max spawns allowed for current subwave
        int spawns = 3 + rand() % 2 + currentWave / 2 + (Globals::Difficulty() > 0 ? 1 : 0);
        spawns = spawns > (int)dynamicSpawnPoints.size() ? (int)dynamicSpawnPoints.size() : spawns;

        // Base value for next spawn time
        nextSpawns = std::max(330 + rand() % 20 - currentWave * (5 + 2 * Globals::Difficulty()), 140);

        set<int> availableSpawnPoints;
        for (int i = 0; i < dynamicSpawnPoints.size(); i++) {
            availableSpawnPoints.insert(i);
        }

        int spawnsThisSubwave = 0;

        int subwaveThreatLevel = 0;
        int maxThreat = 100 + currentWave * (5 + 3 * Globals::Difficulty());

        // Fighter, Swordsman, Zerk - in this order
        array<int, dynamicSpawnListSize> dynamicSpawnsLimits = {
            1337,
            2 + (currentWave >= 4 ? 1 : 0) + (currentWave >= 6 ? 1 : 0),
            2 + (currentWave >= 6 ? 1 : 0)
        };

        // This code may get expanded for other turret types in the deep dark future. For now, there's only Gun Turrets
        int turretLimit = 1 + (currentWave > 2 ? 1 : 0) + (currentWave > 4 ? 1 : 0) + (Globals::Difficulty() > 0 && currentWave > 5 ? 1 : 0);


        // Turret spawns here
        if (!didTurretSpawns && currentWave > 1) {
            didTurretSpawns = true;
            int turretCount = 0;

            // Turrets spawn at start of a wave
            set<int> availableStaticSpawnPoints;
            for (int i = 0; i < staticSpawnPoints.size(); i++) {
                availableStaticSpawnPoints.insert(i);
            }

            while (turretCount < turretLimit) {
                int spawnPoint = rand() % staticSpawnPoints.size();
                int tries = 50;
                while (availableStaticSpawnPoints.find(spawnPoint) == availableStaticSpawnPoints.end() && tries > 0) {
                    tries--;
                    spawnPoint = rand() % staticSpawnPoints.size();
                }

                uint64_t ID = Globals::Game().AddEntity(EntityType::GunTurret, staticSpawnPoints[spawnPoint].first);
                Entity* entity = Globals::Game().GetEntity(ID);
                entity->GetTransform().direction = staticSpawnPoints[spawnPoint].second;
                entitiesToKill.push_back(ID);

                availableStaticSpawnPoints.erase(spawnPoint);
                turretCount++;
            }
        }

        // Actual enemies are spawned here
        while (spawnCredits > 0 && availableSpawnPoints.size() > 0 && spawnsThisSubwave < spawns) {
            vector<int> enemies = { 0 }; // Fighter
            int allowedThreat = maxThreat - subwaveThreatLevel;

            // Swordsmen
            if (currentWave > 2 && allowedThreat > 40 && currentSpawnCount[1] < dynamicSpawnsLimits[1]) {
                enemies.push_back(1);
            }

            // Zerk
            if (currentWave > 4 && allowedThreat > 62 && currentSpawnCount[2] < dynamicSpawnsLimits[2]) {
                enemies.push_back(2);
            }

            int enemy = enemies[rand() % enemies.size()];

            int spawnPoint = rand() % dynamicSpawnPoints.size();
            int tries = 50;
            while (availableSpawnPoints.find(spawnPoint) == availableSpawnPoints.end() && tries > 0) {
                tries--;
                spawnPoint = rand() % dynamicSpawnPoints.size();
            }

            if (enemy == 1 && rand() % 100 < 20) {
                // 20% Chance to replace Swordsman spawn with Fighter
                enemy = 0;
            }

            if (enemy == 2 && rand() % 100 < 20) {
                // Same with Zerk
                enemy = 0;
            }

            EntityType enemyToAdd;
            switch (enemy) {
            case 0: {
                enemyToAdd = EntityType::Fighter;
                spawnCredits -= 120;
                nextSpawns += 60;
                subwaveThreatLevel += 10;
            } break;
            case 1: {
                enemyToAdd = EntityType::Knight;
                spawnCredits -= 240;
                nextSpawns += 170;
                subwaveThreatLevel += 40;
            } break;
            case 2: {
                enemyToAdd = EntityType::Chaser;
                spawnCredits -= 320;
                nextSpawns += 140;
                subwaveThreatLevel += 62;
            } break;
            default:
                std::cout << "Failed a spawn!" << endl;
                spawnsThisSubwave++;
                continue;
                break;
            }

            uint64_t ID = Globals::Game().AddEntity(enemyToAdd, dynamicSpawnPoints[spawnPoint].first);
            Entity* entity = Globals::Game().GetEntity(ID);
            entity->GetTransform().direction = dynamicSpawnPoints[spawnPoint].second;
            currentSpawnCount[enemy]++;

            availableSpawnPoints.erase(spawnPoint);
            spawnsThisSubwave++;
        }
    }

    void LevelDirector::RunBossSpawnLogic() {
        if (!bossSpawned) {
            auto& playerStats = ((Actor*)Globals::ThePlayer())->GetStatsReference();
            playerStats.maxHealth = Utility::ClampValue(playerStats.maxHealth + 50.0, 0.0, 100.0);
            playerStats.health = Utility::ClampValue(playerStats.health + 40.0, 0.0, playerStats.maxHealth);

            bossSpawned = true;
            Globals::Game().AddTheShadow();
            Globals::Game().GetTheShadow()->GetTransform().position = Game::Vector2(800, 200);
            Globals::Game().GetTheShadow()->GetTransform().direction = 170.0;
        }

        Entity* shadow = Globals::Game().GetTheShadow();

        if (shadow == nullptr) {
            return;
        }

        nextSpawns = rand() % 30;

        int difficulty = ((ShadowAI*)((Actor*)shadow)->GetAI())->GetDifficultyLevel();

        bool lastStand = ((ShadowAI*)((Actor*)shadow)->GetAI())->InLastStand();

        int enemy = rand() % 3;

        int spawnPoint = rand() % dynamicSpawnPoints.size();

        int chillFactor = Globals::Game().GetActiveThreat() / 4;

        if (enemy == 1 && rand() % 100 < 20 + chillFactor) {
            // 20% Chance to replace Swordsman spawn with Fighter
            enemy = 0;
        }

        if (enemy == 2 && rand() % 100 < 20 + chillFactor) {
            // Same with Zerk
            enemy = 0;
        }

        EntityType enemyToAdd = EntityType::Fighter;
        switch (enemy) {
        case 0: {
            enemyToAdd = EntityType::Fighter;
            nextSpawns += 100;
        } break;
        case 1: {
            enemyToAdd = EntityType::Knight;
            nextSpawns += 320;
        } break;
        case 2: {
            enemyToAdd = EntityType::Chaser;
            nextSpawns += 180;
        } break;
        }

        uint64_t ID = Globals::Game().AddEntity(enemyToAdd, dynamicSpawnPoints[spawnPoint].first);
        Entity* entity = Globals::Game().GetEntity(ID);
        entity->GetTransform().direction = dynamicSpawnPoints[spawnPoint].second;

        nextSpawns += 420 + difficulty * 2;
    }
}