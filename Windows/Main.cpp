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

    #pragma region Initialization Stuff

    if (!SystemInit()) {
        SystemQuit();
        return -1;
    }

    int prevTime = SDL_GetTicks();

    Game::GameMaster ProjectSilver;
    Game::Globals::SetTheGame(ProjectSilver);
    ProjectSilver.UltimateMegaInitOfDestiny();

    Game::Sprite levelSprite;
    levelSprite.SetTexture("Level");
    levelSprite.SetPosition({ 0, 0 });
    levelSprite.SetCenter({ 0, 0 });
    ProjectSilver.Graphics.AddDrawable(&levelSprite);

    Game::Sprite targetSprite;
    targetSprite.SetTexture("Target");
    targetSprite.SetPosition({ 1266, 668 });
    targetSprite.SetCenter({ 21, 21 });
    targetSprite.SetRelativeToCamera(false);
    ProjectSilver.Graphics.AddDrawable(&targetSprite);
    targetSprite.SetLayer(200);

    Game::BasicText text;
    text.SetText("yro\'ue mom gae xd");
    text.SetFont(&ProjectSilver.Assets, "Big");
    //ProjectSilver.Graphics.AddDrawable(&text);


    Game::BasicText angleDebug;
    angleDebug.SetFont(&ProjectSilver.Assets, "Huge");
    angleDebug.SetRelativeToCamera(false);
    ProjectSilver.Graphics.AddDrawable(&angleDebug);
    
    
    ProjectSilver.Audio.PlayMusic("YourMom");

    /*
    Game::Actor player;
    Game::PlayerPseudoAI playerLogic;
    playerLogic.SetEntity(&player);
    player.SetAI(&playerLogic);
    player.GetComponent().AddAnimation("PlayerIdle");
    player.GetComponent().SetDefaultAnimation("PlayerIdle");
    player.GetComponent().SwitchAnimation("PlayerIdle");
    player.GetComponent().SetCenter({ 48, 67});
    */

    Game::Actor fighter(new Game::FighterAI());
    fighter.GetComponent().AddAnimation("PlayerIdle");
    fighter.GetComponent().SetDefaultAnimation("PlayerIdle");
    fighter.GetComponent().SwitchAnimation("PlayerIdle");
    fighter.GetComponent().SetCenter({ 48, 67 });

    ProjectSilver.AddThePlayer();

    //comp.SwitchToDefault();

    Game::SphereCollider sphereL(Game::Vector2(473, 300), 50, Game::Collider::ColliderType::Static);
    Game::BoxCollider box1L(Game::Vector2(0, 0), 300, 100, Game::Collider::ColliderType::Static);
    Game::BoxCollider box2L(Game::Vector2(0, 0), 100, 600, Game::Collider::ColliderType::Static);
    Game::BoxCollider box3L(Game::Vector2(0, 500), 800, 100, Game::Collider::ColliderType::Static);
    Game::BoxCollider box4L(Game::Vector2(700, 0), 100, 600, Game::Collider::ColliderType::Static);

    ProjectSilver.AddCollider(&sphereL);
    ProjectSilver.AddCollider(&box1L);
    ProjectSilver.AddCollider(&box2L);
    ProjectSilver.AddCollider(&box3L);
    ProjectSilver.AddCollider(&box4L);

    #pragma endregion

    #pragma region Game Loop

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

        if (delta > step) {
            cout << "Lag: " << delta - step << " ";
        }
        
        for (int i = 0; i < bonusUpdates; i++) {
            ProjectSilver.Update(true);
        }

        ProjectSilver.Input.Update();


        
        double speed = 3;
        ProjectSilver.GetThePlayer()->Update();
        fighter.Update();

        //auto prevPos = sphere2.GetPosition();
        ProjectSilver.BuildSpacialHashMap();
        ProjectSilver.ResolveMovementCollisions();
        ProjectSilver.ResolveCombatCollisions();

        ProjectSilver.Graphics.CenterCameraOn(ProjectSilver.GetThePlayer()->GetTransform().position);



        auto var = ProjectSilver.Input.GetMousePosition();
        auto vect = Game::Vector2(var.first - 960.0, var.second - 540.0);
        auto angle = vect.Angle();
        targetSprite.SetPosition({ var.first, var.second });

        

        //cout << angle << endl;
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