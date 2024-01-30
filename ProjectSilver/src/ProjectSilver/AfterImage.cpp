module;

#include <ProjectSilver/PCHeader.hpp>

module ProjectSilver.AfterImage;
import ProjectSilver.Globals;

namespace Game {
	AfterImage::AfterImage():
		framesPerCollumn(-1),
		framesPerRow(-1),
		fadeCounter(0),
		targetFadeCounter(0),
		image(nullptr),
		targetFrame(-1)
	{

	}

	AfterImage::~AfterImage() {}

	void AfterImage::SetStaticTexture(const std::string& texture) {
		auto& lib = Globals::Assets().GetTextureLibrary();
		if (lib.find(texture) == lib.end()) {
			//Error
			return;
		}
		this->image = lib.at(texture).texture.get();
		targetFrame = 0;
		framesPerCollumn = 1;
		framesPerRow = 1;
	}

	void AfterImage::SetAnimationFrame(const std::string& animation, int frame) {
		auto& lib = Globals::Game().GetAnimationLibrary();
		if (lib.find(animation) == lib.end()) {
			//Error
			return;
		}
		auto& info = lib.at(animation).GetInfo();

		std::string textureID = lib.at(animation).GetAnimationTexture();

		auto& libTex = Globals::Assets().GetTextureLibrary();
		if (libTex.find(textureID) == libTex.end()) {
			//Error
			return;
		}

		this->image = libTex.at(textureID).texture.get();
		targetFrame = frame;
		framesPerRow = info.framesPerRow;
		framesPerCollumn = info.framesPerCollumn;
	}

	void AfterImage::Update() {
		if (timeToLive <= 0) {
			return;
		}

		bool fading = fadeCounter <= targetFadeCounter / 1.5;
		double fadeFactor = double(fadeCounter) / (targetFadeCounter / 1.5);

		if (fadeCounter > 0) {
			fadeCounter--;
			if (fading && targetFadeCounter > 0) {
				SetAlpha(uint8_t(GetAlpha() * fadeFactor));
			}
		}

		Effect::Update();
	}

	void AfterImage::Draw() {
		// Uses the same logic from AnimatedSprite

		using namespace Game::Utility;
		if (image == nullptr || targetFrame >= framesPerRow * framesPerCollumn) {
			return;
		}

        auto size = image->getSize();

        int frameWidth  = size.x / framesPerRow;
        int frameHeight = size.y / framesPerCollumn;

        int collumn = targetFrame % framesPerRow;
        int row     = targetFrame / framesPerRow;

        sf::Sprite sprite(*image,
                          sf::IntRect{sf::Vector2i{frameWidth * collumn, frameHeight * row},
                                      sf::Vector2i{frameWidth, frameHeight}});

        sprite.setPosition(
            sf::Vector2f{(float)transform->position.x, (float)transform->position.y});
        sprite.setOrigin(sf::Vector2f{(float)transform->center.x, (float)transform->center.y});
        sprite.setColor(sf::Color{color.r, color.g, color.b, alpha});
        sprite.setRotation(sf::degrees(transform->direction));

        GraphicsEngine::Window->draw(sprite);
		
	}
}