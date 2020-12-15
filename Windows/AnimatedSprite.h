#ifndef ANIMATION_HEADER
#define ANIMATION_HEADER

#include "Sprite.h"

namespace Game {
	class AnimatedSprite : public Sprite {
	public:
		enum LoopMode {
			NormalLoop,
			Static,
			PingPongLoop
		};
	protected:
		int updatesPerFrame; // Number of game frames it takes to switch to a new animation frame
		int framesPerRow;
		int framesPerCollumn;
		int totalFrames;

		LoopMode mode;
		int direction;
		int currentFrame;
		int accumulatedUpdates;
	public:
		AnimatedSprite();

		void SetAnimationInfo(int updatesPerFrame, int framesPerRow, int framesPerCollumn, LoopMode mode = LoopMode::NormalLoop);
		void SetFrame(int frame);
		void Update(int frameUpdates = 1);

		virtual void Draw();
	};
}

#endif