#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    GameMaster* Globals::theGame     = nullptr;
    std::string Globals::gameVersion = "v0.91";

    void Globals::SetTheGame(GameMaster& game)
    {
        theGame = &game;
    }

    void Globals::GameDeaded()
    {
        theGame = nullptr;
    }

    GameMaster& Globals::Game()
    {
        if (theGame == nullptr)
        {
            throw std::invalid_argument("Game was nullptr in Globals::Game()!");
        }
        return *theGame;
    }

    AudioEngine& Globals::Audio()
    {
        if (theGame == nullptr)
        {
            throw std::invalid_argument("Game was nullptr in Globals::Audio()!");
        }
        return theGame->Audio;
    }

    GraphicsEngine& Globals::Graphics()
    {
        if (theGame == nullptr)
        {
            throw std::invalid_argument("Game was nullptr in Globals::Graphics()!");
        }
        return theGame->Graphics;
    }

    AssetManager& Globals::Assets()
    {
        if (theGame == nullptr)
        {
            throw std::invalid_argument("Game was nullptr in Globals::Asset()!");
        }
        return theGame->Assets;
    }

    Entity* Globals::ThePlayer()
    {
        if (theGame == nullptr)
        {
            throw std::invalid_argument("Game was nullptr in Globals::ThePlayer()!");
        }
        return theGame->GetThePlayer();
    }


    GameMaster::DifficultyLevel Globals::Difficulty()
    {
        if (theGame == nullptr)
        {
            throw std::invalid_argument("Game was nullptr in Globals::Difficulty()!");
        }
        return theGame->GetDifficulty();
    }

    void Globals::ChangeDifficulty(GameMaster::DifficultyLevel difficulty)
    {
        if (theGame == nullptr)
        {
            throw std::invalid_argument(
                "Game was nullptr in Globals::ChangeDifficulty()!");
        }
        theGame->SetDifficulty(difficulty);
    }

    std::string Globals::Version()
    {
        return gameVersion;
    }
} // namespace Game