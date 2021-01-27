#ifndef LEVELDIRECTOR_HEADER
#define LEVELDIRECTOR_HEADER

#include "PCHeader.h"
#include "Entity.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "Sprite.h"
#include "BasicText.h"

/*
Notes:
* std::list is used to preserve iterator validity (just in case realocations would be needed in the future)
*/

namespace Game {
	class LevelDirector : public Entity {
    private:
        const static int dynamicSpawnListSize = 3;

        int counter;
        int nextSpawns;
        int resetCounter;
        int audioStartDelay;

        list<Game::BoxCollider> levelBoxes;
        list<Game::SphereCollider> levelSpheres;

        list<Game::Sprite> levelSprites;

        Game::BasicText waveText;
        Game::BasicText difficultyText;

        array<int, dynamicSpawnListSize> currentSpawnCount;

        int currentWave;

        int spawnCredits;

        bool didTurretSpawns;
        bool bossSequence;
        bool bossSpawned;
        bool didBossTurrets;

        vector<uint64_t> entitiesToKill;

        vector<pair<Vector2, double>> dynamicSpawnPoints;
        vector<pair<Vector2, double>> staticSpawnPoints;

        int ressuplyCounter;
    public:
        LevelDirector();
        ~LevelDirector();

        void RunWaveSpawnLogic();
        void RunBossSpawnLogic();

        void KillTrackedEntities();

        virtual void Update();
	};
}

#endif
