#ifndef BASICTEXT_HEADER
#define BASICTEXT_HEADER

#include "PCHeader.h"

#include "Drawable.h"
#include "AssetManager.h"

namespace Game {
	class BasicText : public Drawable {
	public:
		enum TextRenderType {
			ContinuousLeft,
			ContinuousRight,
			ContinuousCentered,
			BlockCentered, // Not implemented
			BlockLeft, // Not implemented
			BlockRight // Not implemented
		};
	protected:
		SDL_Texture* texture;
		string fontID;
		string text;
		int left;
		int right;
		int top;
		int bottom;
		TextRenderType type;
	public:
		BasicText();
		BasicText(const string& fontID, const string& text, TextRenderType type = TextRenderType::ContinuousLeft);
		virtual void Draw();
		void SetFont(const string& ID);
		void SetConstraints(int left, int right, int top, int bottom); // Not implemented
		void SetText(const string& text);

		inline void SetRenderType(TextRenderType type) { this->type = type; }
	};
}

#endif