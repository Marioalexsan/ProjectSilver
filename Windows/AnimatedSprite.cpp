#include "PCHeader.h"

#include "AnimatedSprite.h"
#include "GraphicsEngine.h"

namespace Game {
	AnimatedSprite::AnimatedSprite():
		finished(false),
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
		accumulatedUpdates = 0;
		if (currentFrame >= totalFrames) {
			currentFrame = totalFrames - 1;
		}
	}

	void AnimatedSprite::Restart() {
		direction = 1;
		currentFrame = -1;
		accumulatedUpdates = 0;
		finished = false;
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
					else if(direction == -1) {
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
				case LoopMode::PlayOnce: {
					if (direction != 0) {
						currentFrame++;
						if (currentFrame >= totalFrames) {
							currentFrame = totalFrames - 1;
							direction = 0;
							finished = true;
						}
					}
				} break;
			}
		}
		
		currentFrame = Utility::ClampValue(currentFrame, 0, totalFrames - 1);
	}

	void AnimatedSprite::Draw() {
		using namespace Game::Utility;
		if (texture == nullptr || currentFrame < 0 || currentFrame >= totalFrames) {
			return;
		}
		int w, h;
		SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

		int frameWidth = w / framesPerRow;
		int frameHeight = h / framesPerCollumn;

		int collumn = currentFrame % framesPerRow;
		int row = currentFrame / framesPerRow;

		SDL_Rect src = MakeSDLRect(frameWidth * collumn, frameHeight * row, frameWidth, frameHeight);
		SDL_Rect dest = MakeSDLRect(int(transform->position.x), int(transform->position.y), frameWidth, frameHeight);
		SDL_Point cen = MakeSDLPoint(int(transform->center.x), int(transform->center.y));

		SDL_SetTextureAlphaMod(texture, alpha);
		SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
		GraphicsEngine::RenderCopyExAdvanced(texture, &src, &dest, transform->direction, &cen, SDL_RendererFlip::SDL_FLIP_NONE, relativeToCamera);
	}

}