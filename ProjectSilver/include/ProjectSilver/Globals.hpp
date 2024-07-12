#pragma once

#include <ProjectSilver/GameMaster.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    class Globals
    {
    private:
        static GameMaster* theGame;
        static std::string gameVersion;

    public:
        static void SetTheGame(GameMaster& game);

        static void GameDeaded();

        static GameMaster&                 Game();
        static AudioEngine&                Audio();
        static GraphicsEngine&             Graphics();
        static AssetManager&               Assets();
        static Entity*                     ThePlayer();
        static GameMaster::DifficultyLevel Difficulty();
        static void        ChangeDifficulty(GameMaster::DifficultyLevel difficulty);
        static std::string Version();
    };
} // namespace Game
