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
		type(TextRenderType::ContinuousLeft) {}

	BasicText::BasicText(const string& fontID, const string& text, BasicText::TextRenderType type):
		text(text),
		type(type),
		left(0),
		right(100),
		top(0),
		bottom(100)
	{
		SetFont(fontID);
	}

	int CalculateTextLength(const AssetManager::SpriteFontData* fontData, const string& text) {
		if (text.length() == 0) {
			return 0;
		}

		int pixels = 0;

		pixels += fontData->charLibrary.at(text[0]).xoffset;

		for (int i = 0; i < text.length(); i++) {
			pixels += fontData->charLibrary.at(text[i]).xadvance;
		} 
		pixels += fontData->charLibrary.at(text[text.length() - 1]).width;
		pixels -= fontData->charLibrary.at(text[text.length() - 1]).xadvance;
		pixels -= fontData->charLibrary.at(text[text.length() - 1]).xoffset;
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
			case TextRenderType::ContinuousLeft: 
			case TextRenderType::ContinuousRight:
			case TextRenderType::ContinuousCentered:
			{
				int currentX = 0;
				int wordPos = 0;

				int orientationOffset = 0;

				if (type == TextRenderType::ContinuousCentered) {
					orientationOffset = CalculateTextLength(&data, text) / 2;
				}
				else if (type == TextRenderType::ContinuousRight) {
					orientationOffset = CalculateTextLength(&data, text);
				}

				while (wordPos < text.length()) {
					auto& info = data.charLibrary.at(text[wordPos]);

					SDL_Rect src = Game::Utility::MakeSDLRect(info.x, info.y, info.width, info.height);

					SDL_Rect dest = Game::Utility::MakeSDLRect(int(transform->position.x) + currentX + info.xoffset - orientationOffset, int(transform->position.y) + info.yoffset, info.width, info.height);

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
		auto& lib = manager.GetFontLibrary();
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
