#pragma once

#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/AnimatedSprite.hpp>

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
			std::string param;
		};

		enum AnimationInstruction {
			ExecuteCallback,
			PlaySound,
			SwitchAnimation
		};

		struct Instruction {
			AnimationInstruction what;
			std::string param;

			Instruction(AnimationInstruction what, std::string param) :
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
		};

	protected:
		std::string textureID;
		std::function<void()> callback; // Should rethink what the callback should be
		std::vector<Action> actions;

		Info info;

		Vector2 customCenter;
	public:
		Animation(const std::string& textureID, const std::vector<Action>& actions);
		Animation();

		void SetInfo(const Info& info);
		const Info& GetInfo() const;

		void AddAction(const Action& action);
		const std::vector<Action>& GetActions() const;

		void SetAnimationTexture(const std::string& textureID);
		const std::string& GetAnimationTexture() const;

		inline void SetCustomCenter(const Vector2& center) {
			customCenter = center;
		}

		inline const Vector2& GetCustomCenter() const {
			return customCenter;
		}

		void SetCallback(const std::function<void()>& callback);
		void ExecCallback() const;
	};
}
