#ifndef RENDERCOMPONENT_HEADER
#define RENDERCOMPONENT_HEADER

#include "PCHeader.h"
#include "AnimatedSprite.h"

namespace Game {
	// "Slightly" inspired from Secrets of Grindea

	class Animation {
	public:
		enum AnimationCriteria {
			TriggerAtFrameX,
			TriggerAtEnd,
			TriggerAtStart
		};

		struct Criteria {
			AnimationCriteria what;
			string param;
		};

		enum AnimationInstruction {
			ExecuteCallback,
			PlaySound,
			SwitchAnimation
		};

		struct Instruction {
			AnimationInstruction what;
			string param;
		};

		struct Action {
			Criteria criteria;
			Instruction instruction;
		};

		struct Info {
			int updatesPerFrame;
			int framesPerRow;
			int framesPerCollumn;
			AnimatedSprite::LoopMode mode;

			Info() = default;
		};

	protected:
		string textureID;
		function<void()> callback;
		vector<Action> actions;

		Info info;
	public:
		Animation(const string& textureID, const vector<Action>& actions);
		Animation();

		void SetInfo(const Info& info);
		const Info& GetInfo() const;

		void AddAction(const Action& action);
		const vector<Action>& GetActions() const;

		void SetAnimationTexture(const string& textureID);
		const string& GetAnimationTexture() const;

		void SetCallback(const function<void()>& callback);
		void ExecCallback() const;
	};
}

#endif