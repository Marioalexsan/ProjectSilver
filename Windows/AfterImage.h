#ifndef AFTERIMAGE_HEADER
#define AFTERIMAGE_HEADER

#include "PCHeader.h"
#include "Effect.h"

namespace Game {
	class AfterImage : public Effect {
	protected:
		SDL_Texture* image;
		int targetFrame;

		int framesPerRow;
		int framesPerCollumn;

		int targetFadeCounter;
		int fadeCounter;

	public:
		AfterImage();
		~AfterImage();

		void SetStaticTexture(const string& texture);
		void SetAnimationFrame(const string& animation, int frame = 0);

		inline void SetFadeTime(int fade) {
			fadeCounter = targetFadeCounter = fade;
		}

		virtual void Update();
		virtual void Draw();
	};
}

#endif