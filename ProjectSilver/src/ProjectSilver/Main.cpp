// Project Silver

// TO DO:
// Ensure that if a resource is destroyed (texture, data, etc.), all existin pointers to it are also set to nullptr.

#include <ProjectSilver/PCHeader.hpp>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <chrono>

#include <ProjectSilver/LogHandler.hpp>
#include <ProjectSilver/GameMaster.hpp>
#include <ProjectSilver/Sprite.hpp>
#include <ProjectSilver/BasicText.hpp>
#include <ProjectSilver/AnimatedSprite.hpp>
#include <ProjectSilver/RenderComponent.hpp>
#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/CollisionMaster.hpp>
#include <ProjectSilver/Actor.hpp>
#include <ProjectSilver/PlayerPseudoAI.hpp>
#include <ProjectSilver/FighterAI.hpp>

bool SystemInit() {
    sf::ContextSettings context = sf::ContextSettings(32, 0, 4);
    Game::GraphicsEngine::Window = std::make_unique<sf::RenderWindow>(sf::VideoMode{{1600, 900}},
                                                                      "Project Silver",
                                                                      sf::Style::Default, context);

    
    Game::GraphicsEngine::Window->setMouseCursorVisible(false);

    if (!Game::GraphicsEngine::Window->isOpen()) {
        Game::LogHandler::Log("Failed to create Window!", Game::LogHandler::MessageType::Error);
        return false;
    }

    return true;
}

void SystemQuit() {
    Game::GraphicsEngine::Window->close();
}

int main(int argc, char* args[]) {

    Game::LogHandler::Log("Starting Game", Game::LogHandler::MessageType::Info);

    #pragma region Initialization Stuff

    if (!SystemInit()) {
        Game::LogHandler::Log("Failed to Init System! Exiting.", Game::LogHandler::MessageType::Error);
        SystemQuit();
        return -1;
    }
    srand(unsigned(time(0)));

    Game::GameMaster ProjectSilver;
    Game::Globals::SetTheGame(ProjectSilver);
    ProjectSilver.UltimateMegaInitOfDestiny();

    Game::LogHandler::Log("Inited Game", Game::LogHandler::MessageType::Info);

    // Acts as a cursor; is also used as the player's targeting reticle

    Game::Sprite targetSprite;
    targetSprite.SetTexture("Target");
    targetSprite.SetPosition({ 1266, 668 });
    targetSprite.SetCenter({ 21, 21 });
    targetSprite.SetRelativeToCamera(false);
    targetSprite.RegisterToGame();
    targetSprite.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);

    Game::BasicText FPSCounter;
    FPSCounter.SetPosition({ 1900, 20 });
    FPSCounter.SetRenderType(Game::BasicText::TextRenderType::ContinuousRight);
    FPSCounter.SetFont("Medium");
    FPSCounter.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);
    FPSCounter.SetRelativeToCamera(true);
    FPSCounter.RegisterToGame();

    #pragma endregion

    #pragma region Game Loop

    ProjectSilver.InitMenu();

    bool crash = false;

    std::deque<int> lastFrameTimes;

    bool FPSEnabled = true;

    sf::Time accumulator = sf::Time::Zero;
    sf::Clock clock;
    sf::Time  frameTime = sf::seconds(1) / 60.f;

    while (ProjectSilver.IsGameRunning()) 
    {
        try {
            while (accumulator < frameTime)
            {
                sf::sleep(sf::milliseconds(1));
                accumulator += clock.restart();
            }

            if (accumulator >= frameTime * 3.f)
                accumulator = frameTime * 3.f;

            while (accumulator >= frameTime * 2.f)
            {
                accumulator -= frameTime;
                ProjectSilver.Update(true); // Skips rendering for all bonus updates
            }

            while (accumulator >= frameTime)
            {
                accumulator -= frameTime;
                ProjectSilver.Update(false);
            }

            auto var = ProjectSilver.Input.GetMousePosition();
            targetSprite.SetPosition({ var.first, var.second });

            auto* playerEntity = (Game::Actor*)ProjectSilver.GetThePlayer();
            if (playerEntity)
            {
                Game::PlayerPseudoAI& playerLogic = *(Game::PlayerPseudoAI*)playerEntity->GetAI();

                double recoil = playerLogic.GetRifleRecoil();
                double recoilFactor = std::clamp(std::max(recoil - 3.0, 0.0) / 19.0, 0.0, 1.0);

                targetSprite.SetScale(Game::Vector2(1 + recoilFactor, 1 + recoilFactor));
            }
        }
        catch (std::exception& e) {
            std::string message = "A standard exception occured!\n\tException message: " + std::string(e.what());
            Game::LogHandler::Log(message, Game::LogHandler::MessageType::Error);
            ProjectSilver.Stop();
            crash = true;
        }
        catch (...) {
            std::string message = "An unknown exception occured!";
            Game::LogHandler::Log(message, Game::LogHandler::MessageType::Error);
            ProjectSilver.Stop();
            crash = true;
        }
        

    }

    ProjectSilver.UnloadLevel();
    ProjectSilver.UnloadMenu();
    ProjectSilver.ArmageddonExitProcedures();
    ProjectSilver.Audio.HaltEngine();
    
    #pragma endregion

    if (crash) {
        Game::LogHandler::Log("Exiting Game due to an Error", Game::LogHandler::MessageType::Info);
    }
    else {
        Game::LogHandler::Log("Exiting Game normally", Game::LogHandler::MessageType::Info);
    }

    // Quit SDL
    SystemQuit();
    return 0;
}