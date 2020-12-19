#ifndef BASICTEXT_HEADER
#define BASICTEXT_HEADER

#include "PCHeader.h"

#include "Drawable.h"
#include "AssetManager.h"

namespace Game {
	class BasicText : public Drawable {
	public:
		enum TextRenderType {
			Continuous,
			BlockCentered,
			BlockLeft,
			BlockRight
		};
	protected:
		SDL_Texture* texture;
		AssetManager* manager;
		string fontID;
		string text;
		int left;
		int right;
		int top;
		int bottom;
		TextRenderType type;
	public:
		BasicText();
		virtual void Draw();
		void SetFont(AssetManager* manager, const string& ID);
		void SetConstraints(int left, int right, int top, int bottom);
		void SetText(const string& text);
	};
}

#endif