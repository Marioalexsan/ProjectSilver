#include "Sprite.h"
#include "GraphicsEngine.h"

namespace Game {
	Sprite::Sprite():
		texture(nullptr) {}

	void Sprite::Draw() {
		if (texture == nullptr) {
			// Bad draw
			return;
		}

		uint32_t format;
		int access, w, h;
		SDL_QueryTexture(texture, &format, &access, &w, &h);

		SDL_Rect src = Game::Utility::MakeSDLRect(0, 0, w, h);

		SDL_Rect dest = Game::Utility::MakeSDLRect(position.x, position.y, w, h);

		SDL_Point cen = Game::Utility::MakeSDLPoint(center.x, center.y);

		SDL_RenderCopyEx(Game::GraphicsEngine::Renderer, texture, &src, &dest, rotation, &cen, SDL_RendererFlip::SDL_FLIP_NONE);
	}

	void Sprite::SetTexture(SDL_Texture* texture) {
		this->texture = texture;
	}

	void Sprite::SetTexture(AssetManager* manager, const string& ID) {
		auto lib = manager->GetTextureLibrary();
		if (lib.find(ID) == lib.end()) {
			//Error
			return;
		}
		this->texture = lib.at(ID).texture;
	}
	
	Point Sprite::GetSize() {
		uint32_t format;
		int access, w, h;
		SDL_QueryTexture(texture, &format, &access, &w, &h);
		return { (double)w, (double)h };
	}
}