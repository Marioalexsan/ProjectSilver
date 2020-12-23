#ifndef PLAYER_RENDER
#define PLAYER_RENDER

#include "PCHeader.h"
#include "Animation.h"
#include "AnimatedSprite.h"
#include "Globals.h"

namespace Game {
	class RenderComponent : public AnimatedSprite {
	protected:
		string currentAnimation;
		string defaultAnimation;
		list<string> animations;

		void ParseActions(int startFrame, int endFrame);
		void ActivateAction(Animation::Action action);

		bool parsedStart;
		bool parsedEnd;

		bool switchToDefaultOnEnd;

		bool IsCurrentValid();
		bool IsDefaultValid();

		const Animation& GetCurrentAnimation();
		const Animation& GetDefaultAnimation();
	public:

		RenderComponent();

		void SetDefaultAnimation(const string& ID);

		void AddAnimation(const string& ID);
		void SwitchAnimation(const string& ID);

		void SwitchToDefault();

		const string& GetCurrentAnimationID();

		virtual void Update(int frameupdates = 1);
	};
}

#endif