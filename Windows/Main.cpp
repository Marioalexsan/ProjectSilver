// Project Silver

// TO DO:
// Ensure that if a resource is destroyed (texture, data, etc.), all existin pointers to it are also set to nullptr.

#include "PCHeader.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <chrono>

#include "LogHandler.h"
#include "GameMaster.h"
#include "Sprite.h"
#include "BasicText.h"
#include "AnimatedSprite.h"
#include "RenderComponent.h"
#include "Globals.h"
#include "CollisionMaster.h"
#include "Actor.h"
#include "PlayerPseudoAI.h"
#include "FighterAI.h"

#pragma region Global Variables

// As taboo as it is, one or two of these make things much easier to handle
// If I have time, I'll consider a way to pack these into the GraphicsEngine class

#pragma endregion

#pragma region Utility Functions

bool SystemInit() {
    // Init SDL and SDL_mixer
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
        Game::LogHandler::Log("Failed to Init SDL!", Game::LogHandler::MessageType::Error);
        return false;
    }
    if (Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        Game::LogHandler::Log("Failed to Init SDL Mix!", Game::LogHandler::MessageType::Error);
        return false;
    }
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        Game::LogHandler::Log("Failed to Init SDL Image!", Game::LogHandler::MessageType::Error);
        return false;
    }
    
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    Game::GraphicsEngine::Window = SDL_CreateWindow("Project Silver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0);
    if (Game::GraphicsEngine::Window == nullptr) {
        Game::LogHandler::Log("Failed to create Window!", Game::LogHandler::MessageType::Error);
        return false;
    }
    
    Game::GraphicsEngine::Renderer = SDL_CreateRenderer(Game::GraphicsEngine::Window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
    if (Game::GraphicsEngine::Renderer == nullptr) {
        Game::LogHandler::Log("Failed to create Renderer!", Game::LogHandler::MessageType::Error);
        return false;
    }

    return true;
}

void SystemQuit() {
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();

    if (Game::GraphicsEngine::Renderer != nullptr) {
        SDL_DestroyRenderer(Game::GraphicsEngine::Renderer);
    }
    if (Game::GraphicsEngine::Window != nullptr) {
        SDL_DestroyWindow(Game::GraphicsEngine::Window);
    }
}

#pragma endregion

int main(int argc, char* args[]) {

    Game::LogHandler::Log("Starting Game", Game::LogHandler::MessageType::Info);

    #pragma region Initialization Stuff

    if (!SystemInit()) {
        Game::LogHandler::Log("Failed to Init System! Exiting.", Game::LogHandler::MessageType::Error);
        SystemQuit();
        return -1;
    }
    srand(unsigned(time(0)));

    int prevTime = SDL_GetTicks();
    double accumulatedTimeLoss = 0.0;

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
    FPSCounter.SetRelativeToCamera(false);
    FPSCounter.RegisterToGame();

    #pragma endregion

    #pragma region Game Loop

    ProjectSilver.InitMenu();

    // Scope
    {
        SDL_Rect display;
        SDL_GetDisplayBounds(0, &display);

        Game::LogHandler::Log("Display Resolution: " + std::to_string(display.w) + " x " + std::to_string(display.h), Game::LogHandler::MessageType::Debug);
    }
    // End scope

    bool crash = false;

    std::deque<int> lastFrameTimes;

    bool FPSEnabled = true;

    while (ProjectSilver.IsGameRunning()) 
    {
        try {
            // Wait for next engine frame
            int step = ProjectSilver.fixedTimeStepInMilliseconds;
            if (accumulatedTimeLoss >= 1.0) {
                step -= 1;
                accumulatedTimeLoss -= 1.0;
            }
            int currentTime = SDL_GetTicks();
            int delta = currentTime - prevTime;
            if (delta < step) {
                // Do a small delay
                SDL_Delay((uint32_t)(step - delta));
                currentTime = SDL_GetTicks();
                delta = step;
            }
            //angleDebug.SetText("FPS: " + std::to_string(17 / delta * 60.0));
            int bonusUpdates = (int)round(delta / step) - 1;

            // Do code

            if (bonusUpdates > 0) {
                if (bonusUpdates > 2) {
                    bonusUpdates = 2;
                }
                for (int i = 0; i < bonusUpdates; i++) {
                    ProjectSilver.Update(true); // Skips rendering for all bonus updates
                }
            }

            if (FPSEnabled) {
                int frameCount = 60;
                if (lastFrameTimes.size() >= frameCount) {
                    lastFrameTimes.pop_front();
                }
                lastFrameTimes.push_back(delta);

                int accumulatedTime = 0;
                for (auto elem : lastFrameTimes) {
                    accumulatedTime += elem;
                }


                double FPS = 0.0;
                if (accumulatedTime != 0) {
                   FPS = (16.68 * frameCount) / accumulatedTime * 60.0;
                }
                FPSCounter.SetText("FPS: " + std::to_string((int)FPS));
            }

            ProjectSilver.Update(false);

            //auto prevPos = sphere2.GetPosition();

            auto var = ProjectSilver.Input.GetMousePosition();
            auto vect = Game::Vector2(var.first - 960.0, var.second - 540.0);
            auto angle = vect.Angle();
            targetSprite.SetPosition({ var.first, var.second });

            //cout << angle << endl;
            // Code end

            prevTime = currentTime;

            // Account for SDL_GetTicks() inaccuracy
            accumulatedTimeLoss += 0.333;
        }
        catch (std::exception& e) {
            string message = "A standard exception occured!\n\tException message: " + string(e.what());
            Game::LogHandler::Log(message, Game::LogHandler::MessageType::Error);
            ProjectSilver.Stop();
            crash = true;
        }
        catch (...) {
            string message = "An unknown exception occured!";
            Game::LogHandler::Log(message, Game::LogHandler::MessageType::Error);
            ProjectSilver.Stop();
            crash = true;
        }
        

    }

    ProjectSilver.UnloadLevel();
    ProjectSilver.UnloadMenu();
    ProjectSilver.ArmageddonExitProcedures();
    
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