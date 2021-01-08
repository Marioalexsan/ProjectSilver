#include "PCHeader.h"
#include "LevelDirector.h"
#include "Globals.h"
#include "Actor.h"

namespace Game {
    LevelDirector::LevelDirector() :
        currentCredits(0),
        currentWave(0),
        nextSpawns(0),
        delayedAudioStart(60),
        counter(0),
        resetCounter(0),
        box1L(Game::Vector2(0, 0), 2400, 100, Game::Collider::ColliderType::Static),
        box2L(Game::Vector2(0, 0), 100, 2000, Game::Collider::ColliderType::Static),
        box3L(Game::Vector2(0, 1900), 2400, 100, Game::Collider::ColliderType::Static),
        box4L(Game::Vector2(2300, 0), 100, 2000, Game::Collider::ColliderType::Static),
        sphere1L(Game::Vector2(817, 508), 50, Game::Collider::ColliderType::Static),
        sphere2L(Game::Vector2(1559, 505), 50, Game::Collider::ColliderType::Static),
        sphere3L(Game::Vector2(517, 965), 50, Game::Collider::ColliderType::Static),
        sphere4L(Game::Vector2(859, 1522), 50, Game::Collider::ColliderType::Static),
        sphere5L(Game::Vector2(1612, 1512), 50, Game::Collider::ColliderType::Static),
        sphere6L(Game::Vector2(1835, 957), 50, Game::Collider::ColliderType::Static),
        sphere7L(Game::Vector2(1181, 961), 100, Game::Collider::ColliderType::Static),
        currentSpawnCount(3, 0)
    {
        auto& ProjectSilver = Globals::Game();

        ProjectSilver.Audio.StopMusic();

        ProjectSilver.AddThePlayer();
        ProjectSilver.GetThePlayer()->GetTransform().position = Game::Vector2(800, 200);

        ProjectSilver.AddCollider(&box1L);
        ProjectSilver.AddCollider(&box2L);
        ProjectSilver.AddCollider(&box3L);
        ProjectSilver.AddCollider(&box4L);

        ProjectSilver.AddCollider(&sphere1L);
        ProjectSilver.AddCollider(&sphere2L);
        ProjectSilver.AddCollider(&sphere3L);
        ProjectSilver.AddCollider(&sphere4L);
        ProjectSilver.AddCollider(&sphere5L);
        ProjectSilver.AddCollider(&sphere6L);
        ProjectSilver.AddCollider(&sphere7L);

        waveText.SetFont("Big");
        waveText.SetRelativeToCamera(false);
        waveText.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
        waveText.SetPosition(Vector2(10, 1020));
        waveText.RegisterToGame();
        waveText.SetText("They're coming...");

        levelSpriteFloor.SetTexture("LevelFloor");
        levelSpriteFloor.SetPosition({ 0, 0 });
        levelSpriteFloor.SetCenter({ 0, 0 });
        levelSpriteFloor.SetLayer(Game::GraphicsEngine::CommonLayers::Floor);
        levelSpriteFloor.RegisterToGame();

        levelSpriteWall.SetTexture("LevelWall");
        levelSpriteWall.SetPosition({ 0, 0 });
        levelSpriteWall.SetCenter({ 0, 0 });
        levelSpriteWall.SetLayer(Game::GraphicsEngine::CommonLayers::DefaultHeight);
        levelSpriteWall.RegisterToGame();

        spawnPoints = {
            {Vector2(150, 150), 135.0},
            {Vector2(400, 150), 160.0},
            {Vector2(150, 700), 80.0},
            {Vector2(2250, 150), 225.0},
            {Vector2(2250, 1850), 315.0},
            {Vector2(2250, 1650), 315.0},
            {Vector2(150, 1850), 45.0}
        };

        difficultyText.SetFont("Big");
        difficultyText.SetRelativeToCamera(false);
        difficultyText.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
        difficultyText.SetPosition(Vector2(1720, 1020));
        difficultyText.RegisterToGame();
        switch (Globals::Difficulty()) {
        case 0: {
            difficultyText.SetText("Easy");
            difficultyText.SetColor(Color::White);
        } break;
        case 1: {
            difficultyText.SetText("Normal");
            difficultyText.SetColor(Color::Orange);
        } break;
        case 2: {
            difficultyText.SetText("Hard");
            difficultyText.SetColor(Color::Red);
        } break;
        }
    }

    LevelDirector::~LevelDirector() 
    {
        auto& ProjectSilver = Globals::Game();
        ProjectSilver.RemoveCollider(&box1L);
        ProjectSilver.RemoveCollider(&box2L);
        ProjectSilver.RemoveCollider(&box3L);
        ProjectSilver.RemoveCollider(&box4L);

        ProjectSilver.RemoveCollider(&sphere1L);
        ProjectSilver.RemoveCollider(&sphere2L);
        ProjectSilver.RemoveCollider(&sphere3L);
        ProjectSilver.RemoveCollider(&sphere4L);
        ProjectSilver.RemoveCollider(&sphere5L);
        ProjectSilver.RemoveCollider(&sphere6L);
        ProjectSilver.RemoveCollider(&sphere7L);

        levelSpriteFloor.UnregisterFromGame();
        levelSpriteWall.UnregisterFromGame();

        waveText.UnregisterFromGame();

        difficultyText.UnregisterFromGame();

        ProjectSilver.RemoveThePlayer();
        Globals::Game().RemoveNonSpecialEntities();
    }

    void LevelDirector::Update() {
        if (Globals::ThePlayer() != nullptr) {
            using Actor = Game::Actor;
            Actor* playa = (Actor*)Globals::ThePlayer();
            if (playa->GetStatsReference().isDead) {
                resetCounter++;
                if (resetCounter == 180) {
                    toBeDestroyed = true;
                    Globals::Game().InitMenu();
                }
                return;
            }
        }

        if (delayedAudioStart > 0) {
            delayedAudioStart--;
            if (delayedAudioStart == 0) {
                Globals::Audio().PlayMusic("DigitalGhost");
                Globals::Audio().SetLoopSection("Loop");
            }
        }

        int mercyFactor = Globals::Game().GetActiveThreat() / (45 + Globals::Difficulty() * 5 + currentWave * 5);

        if (rand() % 100 < 100.0 * 3.0 / (mercyFactor + 2.0)) {
            counter++;
        }

        if (counter <= nextSpawns) {
            return;
        }

        if (Globals::Game().GetAliveEnemyCount() <= 0) {
            counter++;
        }

        if (currentCredits <= 0) {
            if (Globals::Game().GetAliveEnemyCount() > 0) {
                return;
            }
            currentWave++;
            currentSpawnCount = std::vector<int>(3, 0);
            currentCredits = 1000 + (currentWave / 2) * 300;
            if (currentWave != 1) {
                waveText.SetText("Wave cleared!");
                Globals::Audio().PlaySound("NextWave");
            }
            counter = 0;
            nextSpawns = 360;
            return;
        }
        waveText.SetText("Wave " + std::to_string(currentWave));

        int spawns = 2 + rand() % 2 + currentWave / 2 + Globals::Difficulty();
        spawns = spawns > spawnPoints.size() ? spawnPoints.size() : spawns;

        nextSpawns = std::max(360 + rand() % 20 - currentWave * (5 + 2 * Globals::Difficulty()), 140);

        set<int> availableSpawnPoints;
        for (int i = 0; i < spawnPoints.size(); i++) {
            availableSpawnPoints.insert(i);
        }
        int currentSpawns = 0;

        int threatLevel = 0;
        int maxThreat = 100 + currentWave * (5 + 3 * Globals::Difficulty());

        vector<int> spawnLimits = {
            1337,
            2 + (currentWave > 3 ? 1 : 0) + (currentWave > 6 ? 1 : 0),
            2 + (currentWave > 5 ? 1 : 0) + (currentWave > 8 ? 1 : 0)
        };

        vector<int>& currentCount = currentSpawnCount;

        while (currentCredits > 0 && availableSpawnPoints.size() > 0 && currentSpawns < spawns) {
            vector<int> enemies = { 0 };
            int allowedThreat = maxThreat - threatLevel;
            if (currentWave > 2 && allowedThreat > 40 && currentCount[1] < spawnLimits[1]) {
                enemies.push_back(1);
            }
            if (currentWave > 4 && allowedThreat > 62 && currentCount[2] < spawnLimits[2]) {
                enemies.push_back(2);
            }
            int enemy = enemies[rand() % enemies.size()];

            int spawnPoint = rand() % spawnPoints.size();
            int tries = 50;
            while (availableSpawnPoints.find(spawnPoint) == availableSpawnPoints.end() && tries > 0) {
                tries--;
                spawnPoint = rand() % spawnPoints.size();
            }

            if (enemy == 1 && rand() % 100 < 25) {
                enemy == 0;
            }

            if (enemy == 2 && rand() % 100 < 25) {
                enemy == 0;
            }

            switch (enemy) {
            case 0: {
                auto ID = Globals::Game().AddNewEnemy(EntityType::Fighter, spawnPoints[spawnPoint].first);
                auto entity = Globals::Game().GetEntity(ID);
                entity->GetTransform().direction = spawnPoints[spawnPoint].second;
                currentCredits -= 120;
                nextSpawns += 40;
                threatLevel += 5;
                currentCount[0]++;
            } break;
            case 1: {
                auto ID = Globals::Game().AddNewEnemy(EntityType::Knight, spawnPoints[spawnPoint].first);
                auto entity = Globals::Game().GetEntity(ID);
                entity->GetTransform().direction = spawnPoints[spawnPoint].second;
                currentCredits -= 240;
                nextSpawns += 170;
                threatLevel += 40;
                currentCount[1]++;
            } break;
            case 2: {
                auto ID = Globals::Game().AddNewEnemy(EntityType::Chaser, spawnPoints[spawnPoint].first);
                auto entity = Globals::Game().GetEntity(ID);
                entity->GetTransform().direction = spawnPoints[spawnPoint].second;
                currentCredits -= 320;
                nextSpawns += 140;
                threatLevel += 62;
                currentCount[2]++;
            } break;
            default:
                std::cout << "Failed a spawn!" << endl;
                break;
            }
            availableSpawnPoints.erase(spawnPoint);
            currentSpawns++;
        }

        counter = 0;
    }
}