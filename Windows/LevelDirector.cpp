#include "PCHeader.h"
#include "LevelDirector.h"
#include "Globals.h"
#include "Actor.h"

namespace Game {
    LevelDirector::LevelDirector() :
        resetCounter(0),
        box1L(Game::Vector2(0, 0), 2400, 100, Game::Collider::ColliderType::Static),
        box2L(Game::Vector2(0, 0), 100, 2000, Game::Collider::ColliderType::Static),
        box3L(Game::Vector2(0, 1900), 2400, 100, Game::Collider::ColliderType::Static),
        box4L(Game::Vector2(2300, 0), 100, 2000, Game::Collider::ColliderType::Static),
        sphere1L(Game::Vector2(817, 508), 50, Game::Collider::ColliderType::Static),
        sphere2L(Game::Vector2(1559, 505), 50, Game::Collider::ColliderType::Static),
        sphere3L(Game::Vector2(517, 965), 50, Game::Collider::ColliderType::Static),
        sphere4L(Game::Vector2(859, 1522), 50, Game::Collider::ColliderType::Static),
        sphere5L(Game::Vector2(1612, 1512), 50, Game::Collider::ColliderType::Static),
        sphere6L(Game::Vector2(1835, 957), 50, Game::Collider::ColliderType::Static),
        sphere7L(Game::Vector2(1181, 961), 100, Game::Collider::ColliderType::Static) 
    {
        auto& ProjectSilver = Globals::Game();

        ProjectSilver.Audio.PlayMusic("YourMom");
        ProjectSilver.AddThePlayer();
        ProjectSilver.GetThePlayer()->GetTransform().position = Game::Vector2(800, 200);

        ProjectSilver.AddCollider(&box1L);
        ProjectSilver.AddCollider(&box2L);
        ProjectSilver.AddCollider(&box3L);
        ProjectSilver.AddCollider(&box4L);

        ProjectSilver.AddCollider(&sphere1L);
        ProjectSilver.AddCollider(&sphere2L);
        ProjectSilver.AddCollider(&sphere3L);
        ProjectSilver.AddCollider(&sphere4L);
        ProjectSilver.AddCollider(&sphere5L);
        ProjectSilver.AddCollider(&sphere6L);
        ProjectSilver.AddCollider(&sphere7L);

        levelSpriteFloor.SetTexture("LevelFloor");
        levelSpriteFloor.SetPosition({ 0, 0 });
        levelSpriteFloor.SetCenter({ 0, 0 });
        levelSpriteFloor.SetLayer(Game::GraphicsEngine::CommonLayers::Floor);
        levelSpriteFloor.RegisterToGame();

        levelSpriteWall.SetTexture("LevelWall");
        levelSpriteWall.SetPosition({ 0, 0 });
        levelSpriteWall.SetCenter({ 0, 0 });
        levelSpriteWall.SetLayer(Game::GraphicsEngine::CommonLayers::DefaultHeight);
        levelSpriteWall.RegisterToGame();
    }

    LevelDirector::~LevelDirector() 
    {
        auto& ProjectSilver = Globals::Game();
        ProjectSilver.RemoveCollider(&box1L);
        ProjectSilver.RemoveCollider(&box2L);
        ProjectSilver.RemoveCollider(&box3L);
        ProjectSilver.RemoveCollider(&box4L);

        ProjectSilver.RemoveCollider(&sphere1L);
        ProjectSilver.RemoveCollider(&sphere2L);
        ProjectSilver.RemoveCollider(&sphere3L);
        ProjectSilver.RemoveCollider(&sphere4L);
        ProjectSilver.RemoveCollider(&sphere5L);
        ProjectSilver.RemoveCollider(&sphere6L);
        ProjectSilver.RemoveCollider(&sphere7L);

        levelSpriteFloor.UnregisterFromGame();
        levelSpriteWall.UnregisterFromGame();

        ProjectSilver.RemoveThePlayer();
        Globals::Game().RemoveNonSpecialEntities();
        Globals::Audio().HaltEngine();
    }

    void LevelDirector::Update() {
        if (Globals::ThePlayer() != nullptr) {
            using Actor = Game::Actor;
            Actor* playa = (Actor*)Globals::ThePlayer();
            if (playa->GetStatsReference().isDead) {
                resetCounter++;
                if (resetCounter == 180) {
                    toBeDestroyed = true;
                    Globals::Game().InitMenu();
                }
            }
        }
    }
}