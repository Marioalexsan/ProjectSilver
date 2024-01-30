module;

#include <ProjectSilver/PCHeader.hpp>
#include <string>

export module ProjectSilver.Globals;
import ProjectSilver.GameMaster;
import ProjectSilver.GraphicsEngine;
import ProjectSilver.AudioEngine;

export namespace Game
{
    class AssetManager;
    class Entity;

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
}