#ifndef ANIMATION_HEADER
#define ANIMATION_HEADER

#include "PCHeader.h"
#include "Sprite.h"

namespace Game {
	class AnimatedSprite : public Sprite {
	public:
		enum LoopMode {
			NormalLoop,
			Static,
			PingPongLoop,
			PlayOnce
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

		bool finished; // Used for animations that don't replay
	public:
		AnimatedSprite();

		void SetAnimationInfo(int updatesPerFrame, int framesPerRow, int framesPerCollumn, LoopMode mode = LoopMode::NormalLoop);
		void SetFrame(int frame);

		void Restart();

		virtual void Update(int frameUpdates = 1);
		virtual void Draw();
	};
}

#endif