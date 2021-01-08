#include "PCHeader.h"

#include "BasicText.h"
#include "GraphicsEngine.h"
#include "Globals.h"

namespace Game {
	BasicText::BasicText():
		texture(nullptr),
		fontID(""),
		left(0),
		right(100),
		top(0),
		bottom(100),
		type(TextRenderType::Continuous) {}

	int CalculateNextWordLength(AssetManager::SpriteFontData* fontData, string word) {
		int pixels = 0;
		for (int i = 0; i < word.length() - 1; i++) {
			pixels += fontData->charLibrary[word[i]].xadvance;
		}
		pixels += fontData->charLibrary[word[word.length() - 1]].width;
		pixels -= fontData->charLibrary[word[word.length() - 1]].xoffset;
		return pixels;
	}

	void BasicText::Draw() {
		auto& lib = Globals::Assets().GetFontLibrary();

		if (lib.find(fontID) == lib.end()) {
			return;
		}

		auto& data = lib.at(fontID).first;

		if (texture == nullptr) {
			// Bad draw
			return;
		}

		if (text.length() == 0) {
			// Nothing to draw
			return;
		}

		int lineHeight = data.arbitraryValues.at("lineHeight");

		switch (type) {
			case TextRenderType::Continuous: 
			{
				int currentX = 0;
				int wordPos = 0;
				while (wordPos < text.length()) {
					auto info = data.charLibrary.at(text[wordPos]);

					SDL_Rect src = Game::Utility::MakeSDLRect(info.x, info.y, info.width, info.height);

					SDL_Rect dest = Game::Utility::MakeSDLRect(int(transform->position.x) + currentX + info.xoffset, int(transform->position.y) + info.yoffset, info.width, info.height);

					SDL_Point cen = Game::Utility::MakeSDLPoint(0, 0);

					SDL_SetTextureAlphaMod(texture, alpha);
					SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
					GraphicsEngine::RenderCopyExAdvanced(texture, &src, &dest, transform->direction, &cen, SDL_RendererFlip::SDL_FLIP_NONE, relativeToCamera);
					wordPos++;
					currentX += info.xadvance;
				}
			}	
			break;
			default:
				return;
		}
	}


	void BasicText::SetFont(const string& ID) {
		AssetManager& manager = Globals::Assets();
		auto lib = manager.GetFontLibrary();
		if (lib.find(ID) == lib.end()) {
			//Error
			return;
		}
		this->texture = lib.at(ID).second.texture;
		this->fontID = ID;
	}

	void BasicText::SetText(const string& text) {
		this->text = text;
	}

}
