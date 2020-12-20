#ifndef PLAYER_RENDER
#define PLAYER_RENDER

#include "PCHeader.h"
#include "Animation.h"
#include "AnimatedSprite.h"
#include "Globals.h"

namespace Game {
	class RenderComponent : public AnimatedSprite {
	protected:
		const Animation* currentInfo;
		const Animation* defaultInfo;
		vector<string> animations;

		void ParseActions(int startFrame, int endFrame);
		void ActivateAction(Animation::Action action);

		bool parsedStart;
		bool parsedEnd;

		bool switchToDefaultOnEnd;
	public:
		RenderComponent();

		void SetDefaultAnimation(const string& ID);

		void AddAnimation(const string& ID);
		void SwitchAnimation(const string& ID);

		virtual void Update(int frameupdates = 1);
	};
}

#endif