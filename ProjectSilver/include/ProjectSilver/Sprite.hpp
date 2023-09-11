#pragma once

#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Drawable.hpp>

namespace Game {
	class Sprite : public Drawable {
	protected:
		sf::Texture* texture;
	public:
		Sprite();

		virtual void SetTexture(const std::string& ID);
		Vector2 GetSize();

		virtual void Draw();
	};
}
