#pragma once

#include <ProjectSilver/BasicText.hpp>
#include <ProjectSilver/BoxCollider.hpp>
#include <ProjectSilver/Entity.hpp>
#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/SphereCollider.hpp>
#include <ProjectSilver/Sprite.hpp>

/*
Notes:
* std::list is used to preserve iterator validity (just in case realocations would be needed in the future)
*/

namespace Game
{
    class LevelDirector : public Entity
    {
    private:
        const static int dynamicSpawnListSize = 3;

        int counter;
        int nextSpawns;
        int resetCounter;
        int audioStartDelay;

        std::list<Game::BoxCollider>    levelBoxes;
        std::list<Game::SphereCollider> levelSpheres;

        std::list<Game::Sprite> levelSprites;

        Game::BasicText waveText;
        Game::BasicText difficultyText;

        std::array<int, dynamicSpawnListSize> currentSpawnCount;

        int currentWave;

        int spawnCredits;

        bool didTurretSpawns;
        bool bossSequence;
        bool bossSpawned;
        bool didBossTurrets;

        std::vector<uint64_t> entitiesToKill;

        std::vector<std::pair<Vector2, double>> dynamicSpawnPoints;
        std::vector<std::pair<Vector2, double>> staticSpawnPoints;

        int ressuplyCounter;

    public:
        LevelDirector();
        ~LevelDirector();

        void RunWaveSpawnLogic();
        void RunBossSpawnLogic();

        void KillTrackedEntities();

        virtual void Update();
    };
} // namespace Game
