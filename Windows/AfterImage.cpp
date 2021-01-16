#include "PCHeader.h"
#include "AfterImage.h"
#include "Globals.h"

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

	void AfterImage::SetStaticTexture(const string& texture) {
		auto& lib = Globals::Assets().GetTextureLibrary();
		if (lib.find(texture) == lib.end()) {
			//Error
			return;
		}
		this->image = lib.at(texture).texture;
		targetFrame = 0;
		framesPerCollumn = 1;
		framesPerRow = 1;
	}

	void AfterImage::SetAnimationFrame(const string& animation, int frame) {
		auto& lib = Globals::Game().GetAnimationLibrary();
		if (lib.find(animation) == lib.end()) {
			//Error
			return;
		}
		auto& info = lib.at(animation).GetInfo();

		string textureID = lib.at(animation).GetAnimationTexture();

		auto& libTex = Globals::Assets().GetTextureLibrary();
		if (libTex.find(textureID) == libTex.end()) {
			//Error
			return;
		}

		this->image = libTex.at(textureID).texture;
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
		int w, h;
		SDL_QueryTexture(image, nullptr, nullptr, &w, &h);

		int frameWidth = w / framesPerRow;
		int frameHeight = h / framesPerCollumn;

		int collumn = targetFrame % framesPerRow;
		int row = targetFrame / framesPerRow;

		SDL_Rect src = MakeSDLRect(frameWidth * collumn, frameHeight * row, frameWidth, frameHeight);
		SDL_Rect dest = MakeSDLRect(int(transform->position.x), int(transform->position.y), frameWidth, frameHeight);
		SDL_Point cen = MakeSDLPoint(int(transform->center.x), int(transform->center.y));

		SDL_SetTextureAlphaMod(image, alpha);
		SDL_SetTextureColorMod(image, color.r, color.g, color.b);
		GraphicsEngine::RenderCopyExAdvanced(image, &src, &dest, transform->direction, &cen, SDL_RendererFlip::SDL_FLIP_NONE, relativeToCamera);
		
	}
}