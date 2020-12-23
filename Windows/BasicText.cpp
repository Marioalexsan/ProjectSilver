#include "PCHeader.h"

#include "BasicText.h"
#include "GraphicsEngine.h"

namespace Game {
	BasicText::BasicText():
		texture(nullptr),
		manager(nullptr),
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
		auto lib = manager->GetFontLibrary();

		if (lib.find(fontID) == lib.end()) {
			return;
		}

		auto data = lib.at(fontID).first;

		if (texture == nullptr) {
			// Bad draw
			return;
		}

		int lineHeight = data.arbitraryValues["lineHeight"];

		switch (type) {
			case TextRenderType::Continuous: 
			{
				int currentX = 0;
				int wordPos = 0;
				while (wordPos < text.length() - 1) {
					auto info = data.charLibrary[text[wordPos]];

					SDL_Rect src = Game::Utility::MakeSDLRect(info.x, info.y, info.width, info.height);

					SDL_Rect dest = Game::Utility::MakeSDLRect(int(transform.position.x) + currentX + info.xoffset, int(transform.position.y) + info.yoffset, info.width, info.height);

					SDL_Point cen = Game::Utility::MakeSDLPoint(0, 0);

					GraphicsEngine::RenderCopyExAdvanced(texture, &src, &dest, transform.direction, &cen, SDL_RendererFlip::SDL_FLIP_NONE, relativeToCamera);
					wordPos++;
					currentX += info.xadvance;
				}
			}	
			break;
			default:
				return;
		}
	}


	void BasicText::SetFont(AssetManager* manager, const string& ID) {
		auto lib = manager->GetFontLibrary();
		if (lib.find(ID) == lib.end()) {
			//Error
			return;
		}
		this->texture = lib.at(ID).second.texture;
		this->fontID = ID;
		this->manager = manager;
	}

	void BasicText::SetText(const string& text) {
		this->text = text;
	}

}
