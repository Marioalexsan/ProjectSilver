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
        cerr << "Error in " << __FILE__ << " at line " << __LINE__ << ": Failed to init SDL." << endl;
        return false;
    }
    if (Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        cerr << "Error in " << __FILE__ << " at line " << __LINE__ << ": Failed to init SDL_mixer." << endl;
        return false;
    }
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        cerr << "Error in " << __FILE__ << " at line " << __LINE__ << ": Failed to init SDL_image." << endl;
        cerr << IMG_GetError() << endl;
        return false;
    }
    
    Game::GraphicsEngine::Window = SDL_CreateWindow("Project Silver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0);
    if (Game::GraphicsEngine::Window == nullptr) {
        return false;
    }
    
    Game::GraphicsEngine::Renderer = SDL_CreateRenderer(Game::GraphicsEngine::Window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
    if (Game::GraphicsEngine::Renderer == nullptr) {
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
        SystemQuit();
        return -1;
    }
    srand(unsigned(time(0)));

    int prevTime = SDL_GetTicks();

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
    ProjectSilver.Graphics.AddDrawable(&targetSprite);
    targetSprite.SetLayer(Game::GraphicsEngine::CommonLayers::GUI);

    #pragma endregion

    #pragma region Game Loop

    ProjectSilver.InitMenu();

    while (ProjectSilver.IsGameRunning()) 
    {
        try {
            // Wait for next engine frame
            double step = ProjectSilver.fixedTimeStep;
            int currentTime = SDL_GetTicks();
            int delta = currentTime - prevTime;
            if ((double)delta < step) {
                // Do a small delay
                SDL_Delay((uint32_t)(step - (double)delta));
                currentTime = SDL_GetTicks();
                delta = (int)step;
            }
            //angleDebug.SetText("FPS: " + std::to_string(17 / delta * 60.0));
            int bonusUpdates = (int)round(delta / step) - 1;

            // Do code

            if (bonusUpdates > 0) {
                if (bonusUpdates > 2) {
                    bonusUpdates = 2;
                }
                cout << "Lag: " << delta - step << " ";
                for (int i = 0; i < bonusUpdates; i++) {
                    ProjectSilver.Update(true); // Skips rendering for all bonus updates
                }
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
        }
        catch (std::exception& e) {
            string message = "A standard exception occured!\n\tException message: " + string(e.what());
            Game::LogHandler::Log(message, Game::LogHandler::MessageType::Error);
            ProjectSilver.Stop();
        }
        catch (...) {
            string message = "An unknown exception occured!";
            Game::LogHandler::Log(message, Game::LogHandler::MessageType::Error);
            ProjectSilver.Stop();
        }
        

    }
    ProjectSilver.UnloadLevel();
    ProjectSilver.UnloadMenu();
    ProjectSilver.ArmageddonExitProcedures();
    
    #pragma endregion

    // Quit SDL
    SystemQuit();
    return 0;
}