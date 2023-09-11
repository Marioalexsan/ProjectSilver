#pragma once

#include <ProjectSilver/PCHeader.hpp>

#include <ProjectSilver/Drawable.hpp>
#include <ProjectSilver/AssetManager.hpp>

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
		sf::Texture* texture;
		std::string fontID;
		std::string text;
		int left;
		int right;
		int top;
		int bottom;
		TextRenderType type;
	public:
		BasicText();
		BasicText(const std::string& fontID, const std::string& text, TextRenderType type = TextRenderType::ContinuousLeft);
		virtual void Draw();
		void SetFont(const std::string& ID);
		void SetText(const std::string& text);

		inline void SetRenderType(TextRenderType type) { this->type = type; }
	};
}
