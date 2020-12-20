// Project Silver

// TO DO:
// Ensure that if a resource is destroyed (texture, data, etc.), all existin pointers to it are also set to nullptr.

#include "PCHeader.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "GameMaster.h"
#include "Sprite.h"
#include "BasicText.h"
#include "AnimatedSprite.h"
#include "RenderComponent.h"
#include "Globals.h"

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
    Game::Globals::SetTheGame(ProjectSilver);

    int prevTime = SDL_GetTicks();

    ProjectSilver.Assets.LoadTexture("Checks", "Untitled.png");
    ProjectSilver.Assets.LoadTexture("Test", "Untitled2.png");
    ProjectSilver.Assets.LoadTexture("Char", "Char.png");

    ProjectSilver.Assets.LoadTexture("Target", "target.png");
    ProjectSilver.Assets.LoadSpriteFont("Huge", "Fonts/CourierNewHuge_0.png", "Fonts/CourierNewHuge.fnt");
    ProjectSilver.Assets.LoadSpriteFont("Big", "Fonts/CourierNewBig_0.png", "Fonts/CourierNewBig.fnt");

    Game::Sprite sprite2;
    sprite2.SetTexture("Checks");
    sprite2.MoveTo({ 1820, 980 });
    sprite2.SetCenter({ 0, 0 });
    ProjectSilver.Graphics.AddDrawable(&sprite2);

    Game::Sprite sprite4;
    sprite4.SetTexture("Target");
    sprite4.MoveTo({ 1266, 668 });
    sprite4.SetCenter({ 21, 21 });
    sprite4.SetRelativeToCamera(false);
    ProjectSilver.Graphics.AddDrawable(&sprite4);
    sprite4.SetLayer(-1);

    Game::Sprite sprite5;
    sprite5.SetTexture("Char");
    sprite5.MoveTo({ 200, 200 });
    sprite5.SetCenter({ 50, 70 });
    ProjectSilver.Graphics.AddDrawable(&sprite5);
    sprite5.SetLayer(255);

    Game::BasicText text;
    text.SetText("yro\'ue mom gae xd");
    text.SetFont(&ProjectSilver.Assets, "Big");
    ProjectSilver.Graphics.AddDrawable(&text);

    Game::BasicText angleDebug;
    angleDebug.SetFont(&ProjectSilver.Assets, "Huge");
    angleDebug.SetRelativeToCamera(false);
    ProjectSilver.Graphics.AddDrawable(&angleDebug);
    
    ProjectSilver.Assets.LoadMusic("YourMom", "digi.ogg");
    ProjectSilver.Assets.LoadSound("Mooz", "Mooz.ogg");
    ProjectSilver.Audio.PlayMusic("YourMom");

    ProjectSilver.Graphics.SetDisplayMode(ProjectSilver.Graphics.VideoModes.at("1920.1080.f"));

    ProjectSilver.Input.SetMouseGrab(true);

    vector<Game::Animation::Action> actio = {
        {{Game::Animation::AnimationCriteria::TriggerAtStart, ""}, {Game::Animation::AnimationInstruction::PlaySound, "Mooz"}},
        {{Game::Animation::AnimationCriteria::TriggerAtEnd, ""}, {Game::Animation::AnimationInstruction::PlaySound, "Mooz"}},
        {{Game::Animation::AnimationCriteria::TriggerAtFrameX, "2"}, {Game::Animation::AnimationInstruction::PlaySound, "Mooz"}}
    };
    ProjectSilver.AddAnimation("BoxThing", Game::Animation("Test", actio));
    ProjectSilver.SetAnimationInfo("BoxThing", { 40, 4, 1, Game::AnimatedSprite::LoopMode::PlayOnce });

    Game::RenderComponent comp;
    comp.AddAnimation("BoxThing");
    comp.SetDefaultAnimation("BoxThing");
    ProjectSilver.Graphics.AddDrawable(&comp);
    comp.SwitchAnimation("BoxThing");

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

        comp.Update();
        ProjectSilver.Input.Update();

        ProjectSilver.Graphics.CenterCameraOn(sprite5.GetPosition());

        using KeyCode = Game::InputHandler::KeyCode;
        using ButtonCode = Game::InputHandler::ButtonCode;
        if (ProjectSilver.Input.IsKeyDown(KeyCode::W)) {
            ProjectSilver.Graphics.PushCamera({ 0, -10 });
            sprite5.PushBy({ 0, -10 });
        }
        if (ProjectSilver.Input.IsKeyDown(KeyCode::S)) {
            ProjectSilver.Graphics.PushCamera({ 0, 10 });
            sprite5.PushBy({ 0, 10 });
        }
        if (ProjectSilver.Input.IsKeyDown(KeyCode::A)) {
            ProjectSilver.Graphics.PushCamera({ -10, 0 });
            sprite5.PushBy({ -10, 0 });
        }
        if (ProjectSilver.Input.IsKeyDown(KeyCode::D)) {
            ProjectSilver.Graphics.PushCamera({ 10, 0 });
            sprite5.PushBy({ 10, 0 });
        }
        if (ProjectSilver.Input.IsButtonPressedThisFrame(ButtonCode::Middle) || ProjectSilver.Input.WasQuitCalled()) {
            ProjectSilver.Stop();
        }

        sprite5.RotateBy(1);

        auto var = ProjectSilver.Input.GetMousePosition();
        auto angle = Game::Vector2(var.first - 960.0, var.second - 540.0).Angle();
        sprite5.SetAngle(angle);
        sprite4.MoveTo({ var.first, var.second });

        cout << angle << endl;
        angleDebug.SetText("Angle: " + std::to_string(angle));
        // Code end

        prevTime = currentTime;

        ProjectSilver.Graphics.RenderAll();
    }

    #pragma endregion

    // Quit SDL
    SystemQuit();
    return 0;
}