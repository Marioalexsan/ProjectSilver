#include "PCHeader.h"
#include "ShadowHPBar.h"
#include "Globals.h"

namespace Game {
	ShadowHPBar::ShadowHPBar():
		HPtex(nullptr),
		lastStandHPtex(nullptr),
		HPValue(0),
		lastStandHPValue(0),
		maxHP(0) {}

	void ShadowHPBar::SetHPTexture(const string& ID) {
		auto& lib = Globals::Assets().GetTextureLibrary();
		if (lib.find(ID) == lib.end()) {
			//Error
			return;
		}
		this->HPtex = lib.at(ID).texture;
	}

	void ShadowHPBar::SetLastStandHPTexture(const string& ID) {
		auto& lib = Globals::Assets().GetTextureLibrary();
		if (lib.find(ID) == lib.end()) {
			//Error
			return;
		}
		this->lastStandHPtex = lib.at(ID).texture;
	}

	void ShadowHPBar::SetHPValues(double HP, double lastStandHP, double maxHP) {
		HPValue = HP;
		lastStandHPValue = lastStandHP;
		this->maxHP = maxHP;
	}

	void ShadowHPBar::Draw() {
		using namespace Game::Utility;
		if (texture == nullptr || HPtex == nullptr || lastStandHPtex == nullptr) {
			return;
		}

		double HPRatio = 0.0;
		double LastStandHPRatio = 0.0;
		if (maxHP > 0.0) {
			HPRatio = abs(HPValue / maxHP);
			LastStandHPRatio = abs(lastStandHPValue / maxHP);
		}

		// Border
		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

		SDL_Rect src = MakeSDLRect(0, 0, width, height);
		SDL_Rect dest = MakeSDLRect(int(transform->position.x), int(transform->position.y), width, height);
		SDL_Point cen = MakeSDLPoint(int(transform->center.x), int(transform->center.y));
		SDL_SetTextureAlphaMod(texture, alpha);
		SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
		GraphicsEngine::RenderCopyExAdvanced(texture, &src, &dest, transform->direction, &cen, SDL_RendererFlip::SDL_FLIP_NONE, relativeToCamera);

		// Last Stand HP
		int w, h;
		SDL_QueryTexture(lastStandHPtex, nullptr, nullptr, &w, &h);

		src = MakeSDLRect(0, 0, w, h);
		dest = MakeSDLRect(int(transform->position.x) + 2, int(transform->position.y) + 2, w * (width - 4) * LastStandHPRatio, h);
		cen = MakeSDLPoint(int(transform->center.x), int(transform->center.y));
		SDL_SetTextureAlphaMod(lastStandHPtex, alpha);
		SDL_SetTextureColorMod(lastStandHPtex, color.r, color.g, color.b);
		GraphicsEngine::RenderCopyExAdvanced(lastStandHPtex, &src, &dest, transform->direction, &cen, SDL_RendererFlip::SDL_FLIP_NONE, relativeToCamera);


		// HP
		SDL_QueryTexture(HPtex, nullptr, nullptr, &w, &h);

		src = MakeSDLRect(0, 0, w, h);
		dest = MakeSDLRect(int(transform->position.x) + 2, int(transform->position.y) + 2, w * (width - 4) * HPRatio, h);
		cen = MakeSDLPoint(int(transform->center.x), int(transform->center.y));
		SDL_SetTextureAlphaMod(HPtex, alpha);
		SDL_SetTextureColorMod(HPtex, color.r, color.g, color.b);
		GraphicsEngine::RenderCopyExAdvanced(HPtex, &src, &dest, transform->direction, &cen, SDL_RendererFlip::SDL_FLIP_NONE, relativeToCamera);

		
	}
}