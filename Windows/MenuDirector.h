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
		// Play
		Game::BoxCollider button1Col;
		Game::Sprite button1BG;
		Game::BasicText button1Text;

		// Toggle difficulty
		Game::BoxCollider button2Col;
		Game::Sprite button2BG;
		Game::BasicText button2Text;
		Game::BasicText button2Diff;

		// Quit
		Game::BoxCollider button3Col;
		Game::Sprite button3BG;
		Game::BasicText button3Text;

		Game::BasicText gameTitle;
	public:
		MenuDirector();
		~MenuDirector();

		virtual void Update();
	};
}

#endif