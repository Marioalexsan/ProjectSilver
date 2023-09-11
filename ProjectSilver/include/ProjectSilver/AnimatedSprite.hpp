#pragma once

#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Sprite.hpp>

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

		inline int GetFrame() { return this->currentFrame; }

		inline int GetFrameCount() { return this->totalFrames; }

		inline int GetAccumulatedUpdates() { return this->accumulatedUpdates; }

		inline int GetUpdatesPerFrame() { return this->updatesPerFrame; }

		inline int GetUpdatesCount() { return this->totalFrames * this->updatesPerFrame; }

		void Restart();

		virtual void Update(int frameUpdates = 1);
		virtual void Draw();
	};
}
