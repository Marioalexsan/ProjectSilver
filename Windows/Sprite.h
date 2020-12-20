#ifndef SPRITE_HEADER
#define SPRITE_HEADER

#include "PCHeader.h"
#include "Drawable.h"

namespace Game {
	class Sprite : public Drawable {
	protected:
		SDL_Texture* texture;
	public:
		Sprite();

		virtual void SetTexture(const string& ID);
		Vector2 GetSize();

		virtual void Draw();
	};
}

#endif