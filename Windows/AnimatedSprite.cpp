#include "AnimatedSprite.h"
#include "GraphicsEngine.h"

namespace Game {
	AnimatedSprite::AnimatedSprite():
		framesPerCollumn(0),
		framesPerRow(0),
		totalFrames(0),
		updatesPerFrame(0),
		currentFrame(-1),
		direction(1),
		accumulatedUpdates(0),
		mode(LoopMode::NormalLoop) {}

	void AnimatedSprite::SetAnimationInfo(int updatesPerFrame, int framesPerRow, int framesPerCollumn, LoopMode mode) {
		this->updatesPerFrame = updatesPerFrame;
		this->framesPerCollumn = framesPerCollumn;
		this->framesPerRow = framesPerRow;
		totalFrames = framesPerCollumn * framesPerRow;
		this->mode = mode;
	}

	void AnimatedSprite::SetFrame(int frame) {
		currentFrame = frame;
		if (currentFrame >= totalFrames) {
			currentFrame = totalFrames - 1;
		}
	}

	void AnimatedSprite::Update(int frameUpdates) {
		accumulatedUpdates++;
		if (accumulatedUpdates >= updatesPerFrame) {
			accumulatedUpdates -= updatesPerFrame;

			switch (mode) {
				case LoopMode::PingPongLoop: {
					if (direction == 1) {
						currentFrame++;
						if (currentFrame >= totalFrames) {
							currentFrame -= 2;
							direction = -1;
						}
					}
					else {
						currentFrame--;
						if (currentFrame < 0) {
							currentFrame += 2;
							direction = 1;
						}
					}
				} break;
				case LoopMode::NormalLoop: {
					currentFrame++;
					if (currentFrame >= totalFrames) {
						currentFrame = 0;
					}
				} break;
			}
		}
		

		if (currentFrame < 0) {
			currentFrame = 0;
		}
		if (currentFrame >= totalFrames) {
			currentFrame = totalFrames - 1;
		}
	}

	void AnimatedSprite::Draw() {
		if (texture == nullptr || currentFrame < 0 || currentFrame >= totalFrames) {
			// Bad draw
			return;
		}

		uint32_t format;
		int access, w, h;
		SDL_QueryTexture(texture, &format, &access, &w, &h);

		int frameWidth = w / framesPerRow;
		int frameHeight = h / framesPerCollumn;

		int collumn = currentFrame % framesPerRow;
		int row = currentFrame / framesPerRow;

		SDL_Rect src = Game::Utility::MakeSDLRect(frameWidth * collumn, frameHeight * row, frameWidth, frameHeight);

		SDL_Rect dest = Game::Utility::MakeSDLRect(position.x, position.y, frameWidth, frameHeight);

		SDL_Point cen = Game::Utility::MakeSDLPoint(center.x, center.y);

		SDL_RenderCopyEx(Game::GraphicsEngine::Renderer, texture, &src, &dest, rotation, &cen, SDL_RendererFlip::SDL_FLIP_NONE);
	}
}