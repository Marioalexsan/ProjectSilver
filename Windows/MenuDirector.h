#ifndef MENUDIRECTOR_HEADER
#define MENUDIRECTOR_HEADER

#include "PCHeader.h"
#include "Entity.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "Sprite.h"
#include "BasicText.h"

namespace Game {
	class MenuDirector : public Entity {
	private:
		Game::BoxCollider button1Col;
		Game::Sprite button1BG;
		Game::BasicText button1Text;

		Game::BoxCollider button2Col;
		Game::Sprite button2BG;
		Game::BasicText button2Text;

		Game::BasicText gameTitle;
	public:
		MenuDirector();
		~MenuDirector();

		virtual void Update();
	};
}

#endif