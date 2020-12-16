// Project Silver

#include "PCHeader.h"

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
    ProjectSilver.Assets.LoadTexture("Char", "Char.png");

    ProjectSilver.Assets.LoadTexture("Target", "target.png");

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
    sprite3.MoveTo({ 1180, 620 });
    sprite3.SetCenter({ 0, 0 });
    ProjectSilver.Graphics.AddDrawable(&sprite3);

    Game::Sprite sprite4;
    sprite4.SetTexture(&ProjectSilver.Assets, "Target");
    sprite4.MoveTo({ 1266, 668 });
    sprite4.SetCenter({ 21, 21 });
    ProjectSilver.Graphics.AddDrawable(&sprite4);

    Game::Sprite sprite5;
    sprite5.SetTexture(&ProjectSilver.Assets, "Char");
    sprite5.MoveTo({ 200, 200 });
    sprite5.SetCenter({ 50, 70 });
    ProjectSilver.Graphics.AddDrawable(&sprite5);

    ProjectSilver.Graphics.SetDisplayMode(ProjectSilver.Graphics.VideoModes.at("1600.900.w"));

    ProjectSilver.Input.SetMouseGrab(true);

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
        int bonusUpdates = (int)round(delta / step) - 1;

        // Do code
        
        for (int i = 0; i < bonusUpdates; i++) {
            ProjectSilver.Update(true);
        }

        animation.Update();
        ProjectSilver.Input.Update();

        using KeyCode = Game::InputHandler::KeyCode;
        using ButtonCode = Game::InputHandler::ButtonCode;
        if (ProjectSilver.Input.IsKeyDown(KeyCode::W)) {
            ProjectSilver.Graphics.PushCamera(0, -10);
        }
        if (ProjectSilver.Input.IsKeyDown(KeyCode::S)) {
            ProjectSilver.Graphics.PushCamera(0, 10);
        }
        if (ProjectSilver.Input.IsKeyDown(KeyCode::A)) {
            ProjectSilver.Graphics.PushCamera(-10, 0);
        }
        if (ProjectSilver.Input.IsKeyDown(KeyCode::D)) {
            ProjectSilver.Graphics.PushCamera(10, 0);
        }
        if (ProjectSilver.Input.IsButtonPressedThisFrame(ButtonCode::Middle) || ProjectSilver.Input.WasQuitCalled()) {
            ProjectSilver.Stop();
        }
        if (ProjectSilver.Input.IsButtonPressedThisFrame(ButtonCode::Right)) {

            sprite5.SetCenter({ 150, 150 });
        }

        animation.PushBy({ 0.5, 0.5 });
        sprite5.RotateBy(1);

        auto var = ProjectSilver.Input.GetMousePosition();
        sprite5.SetAngle(Game::Point(var.first - 200.0, var.second - 200.0).Angle());
        sprite4.MoveTo({ var.first, var.second });

        cout << Game::Point(var.first - 200.0, var.second - 200.0).Angle() << endl;
        // Code end

        prevTime = currentTime;

        ProjectSilver.Graphics.RenderAll();
    }

    #pragma endregion

    // Quit SDL
    SystemQuit();
    return 0;
}