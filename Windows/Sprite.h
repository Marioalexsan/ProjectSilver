#ifndef SPRITE_HEADER
#define SPRITE_HEADER

#include "PCHeader.h"

#include "Drawable.h"
#include "AssetManager.h"

namespace Game {
	class Sprite : public Drawable {
	protected:
		SDL_Texture* texture;
	public:
		Sprite();
		virtual void Draw();
		virtual void SetTexture(SDL_Texture* texture);
		virtual void SetTexture(AssetManager* manager, const string& ID);
		Point GetSize();
	};
}

#endif