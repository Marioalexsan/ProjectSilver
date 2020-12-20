#include "PCHeader.h"

#include "Sprite.h"
#include "Globals.h"

namespace Game {
	Sprite::Sprite():
		texture(nullptr) {}

	void Sprite::SetTexture(const string& ID) {
		auto lib = Globals::Assets().GetTextureLibrary();
		if (lib.find(ID) == lib.end()) {
			//Error
			return;
		}
		this->texture = lib.at(ID).texture;
	}

	Vector2 Sprite::GetSize() {
		int w, h;
		SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
		return { (double)w, (double)h };
	}

	void Sprite::Draw() {
		using namespace Game::Utility;
		if (texture == nullptr) {
			return;
		}

		int w, h;
		SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

		SDL_Rect src = MakeSDLRect(0, 0, w, h);
		SDL_Rect dest = MakeSDLRect(int(position.x), int(position.y), w, h);
		SDL_Point cen = MakeSDLPoint(int(center.x), int(center.y));

		GraphicsEngine::RenderCopyExAdvanced(texture, &src, &dest, rotation, &cen, SDL_RendererFlip::SDL_FLIP_NONE, relativeToCamera);
	}

	
}