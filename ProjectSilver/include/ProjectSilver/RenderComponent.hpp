#pragma once

#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Animation.hpp>
#include <ProjectSilver/AnimatedSprite.hpp>
#include <ProjectSilver/Globals.hpp>

namespace Game {
	class RenderComponent : public AnimatedSprite {
	protected:
		std::string currentAnimation;
		std::string defaultAnimation;
		std::list<std::string> animations;

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

		void SetDefaultAnimation(const std::string& ID);

		void AddAnimation(const std::string& ID);
		void SwitchAnimation(const std::string& ID);

		void SwitchToDefault();

		const std::string& GetCurrentAnimationID();

		virtual void Update(int frameupdates = 1);
	};
}
