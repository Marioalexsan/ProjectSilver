#ifndef LEVELDIRECTOR_HEADER
#define LEVELDIRECTOR_HEADER

#include "PCHeader.h"
#include "Entity.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "Sprite.h"
#include "BasicText.h"

namespace Game {
	class LevelDirector : public Entity {
    private:

        int resetCounter;
        Game::BoxCollider box1L;
        Game::BoxCollider box2L;
        Game::BoxCollider box3L;
        Game::BoxCollider box4L;


        Game::SphereCollider sphere1L;
        Game::SphereCollider sphere2L;
        Game::SphereCollider sphere3L;
        Game::SphereCollider sphere4L;
        Game::SphereCollider sphere5L;
        Game::SphereCollider sphere6L;
        Game::SphereCollider sphere7L;

        Game::Sprite levelSpriteFloor;
        Game::Sprite levelSpriteWall;

        
    public:
        LevelDirector();
        ~LevelDirector();

        virtual void Update();
	};
}

#endif
