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

			Instruction(AnimationInstruction what, string param) :
				what(what),
				param(param) {}

			Instruction() = default;
		};

		struct Action {
			Criteria criteria;
			Instruction instruction;

			Action(Criteria criteria, Instruction instruction):
				criteria(criteria),
				instruction(instruction) {}

			Action() = default;
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

		Vector2 customCenter;
	public:
		Animation(const string& textureID, const vector<Action>& actions);
		Animation();

		void SetInfo(const Info& info);
		const Info& GetInfo() const;

		void AddAction(const Action& action);
		const vector<Action>& GetActions() const;

		void SetAnimationTexture(const string& textureID);
		const string& GetAnimationTexture() const;

		inline void SetCustomCenter(const Vector2& center) {
			customCenter = center;
		}

		inline const Vector2& GetCustomCenter() const {
			return customCenter;
		}

		void SetCallback(const function<void()>& callback);
		void ExecCallback() const;
	};
}

#endif