// SDL2 Hello, World!
// This should display a white screen for 2 seconds
// compile with: clang++ main.cpp -o hello_sdl2 -lSDL2
// run with: ./hello_sdl2
#include <SDL.h>
#include <stdio.h>
#include <SDL_mixer.h>
//#include <SDL_image.h>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "GameMaster.h"
#include "Sprite.h"
#include "AnimatedSprite.h"

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
    
    Game::GraphicsEngine::Window = SDL_CreateWindow("Project Silver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, SDL_WINDOW_OPENGL);
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

    if (!SystemInit()) {
        SystemQuit();
        return -1;
    }

    #pragma region Game Loop
    
    Game::GameMaster ProjectSilver;

    int prevTime = SDL_GetTicks();

    ProjectSilver.Assets.LoadTexture("Checks", "Untitled.png");
    ProjectSilver.Assets.LoadTexture("Test", "Untitled2.png");

    Game::AnimatedSprite animation;
    animation.SetAnimationInfo(12, 4, 1, Game::AnimatedSprite::LoopMode::PingPongLoop);
    animation.SetTexture(&ProjectSilver.Assets, "Test");
    ProjectSilver.Graphics.AddDrawable(&animation);
    animation.MoveTo({ 0, 0 });
    animation.SetCenter({ 50, 50 });

    Game::Sprite sprite;
    sprite.SetTexture(&ProjectSilver.Assets, "Checks");
    sprite.MoveTo({ 0, 0 });
    sprite.SetCenter({ 50, 50 });
    ProjectSilver.Graphics.AddDrawable(&sprite);

    Game::Sprite sprite2;
    sprite2.SetTexture(&ProjectSilver.Assets, "Checks");
    sprite2.MoveTo({ 1820, 980 });
    sprite2.SetCenter({ 0, 0 });
    ProjectSilver.Graphics.AddDrawable(&sprite2);

    Game::Sprite sprite3;
    sprite3.SetTexture(&ProjectSilver.Assets, "Checks");
    sprite3.MoveTo({ 1720, 880 });
    sprite3.SetCenter({ 0, 0 });
    ProjectSilver.Graphics.AddDrawable(&sprite3);

    Game::Sprite sprite4;
    sprite4.SetTexture(&ProjectSilver.Assets, "Checks");
    sprite4.MoveTo({ 1620, 780 });
    sprite4.SetCenter({ 0, 0 });
    ProjectSilver.Graphics.AddDrawable(&sprite4);

    Game::Sprite sprite5;
    sprite5.SetTexture(&ProjectSilver.Assets, "Checks");
    sprite5.MoveTo({ 1520, 680 });
    sprite5.SetCenter({ 0, 0 });
    ProjectSilver.Graphics.AddDrawable(&sprite5);

    while (ProjectSilver.IsGameRunning()) 
    {
        // Wait for next engine frame
        double step = ProjectSilver.fixedTimeStep;
        int currentTime = SDL_GetTicks();
        int delta = currentTime - prevTime;
        if ((double)delta < step) {
            // Do a small delay
            SDL_Delay((uint32_t)(step - (double)delta));
            currentTime = SDL_GetTicks();
        }
        int bonusUpdates = round(delta / step) - 1;

        // Do code
        
        for (int i = 0; i < bonusUpdates; i++) {
            ProjectSilver.Update(true);
        }

        if (prevTime < 3000 && currentTime >= 3000) {
            ProjectSilver.Graphics.SetDisplayMode(Game::GraphicsEngine::VideoModes.at("1600.900.f"));
        }
        if (prevTime < 7000 && currentTime >= 7000) {
            ProjectSilver.Graphics.SetDisplayMode(Game::GraphicsEngine::VideoModes.at("1366.768.w"));
        }
        if (prevTime < 11000 && currentTime >= 11000) {
            ProjectSilver.Graphics.SetDisplayMode(Game::GraphicsEngine::VideoModes.at("1600.900.w"));
        }

        animation.Update();
        animation.PushBy({ 0.5, 0.5 });
        animation.RotateBy(0.1);

        // Code end

        prevTime = currentTime;

        ProjectSilver.Graphics.RenderAll();
        SDL_Event trash;

        SDL_PollEvent(&trash);
    }

    #pragma endregion

    // Quit SDL
    SystemQuit();
    return 0;
}