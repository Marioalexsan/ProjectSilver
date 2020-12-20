#include "PCHeader.h"
#include "Animation.h"

namespace Game {
	Animation::Animation(const string& textureID, const vector<Action>& actions) {
		this->textureID = textureID;
		this->actions = actions;
		callback = nullptr;
	}

	Animation::Animation():
		textureID(""),
		callback(nullptr),
		actions(),
		info() {}


	void Animation::SetInfo(const Info& info) {
		this->info.updatesPerFrame = info.updatesPerFrame;
		this->info.framesPerRow = info.framesPerRow;
		this->info.framesPerCollumn = info.framesPerCollumn;
		this->info.mode = info.mode;
	}

	const Animation::Info& Animation::GetInfo() const {
		return info;
	}

	void Animation::AddAction(const Action& action) {
		actions.push_back(action);
	}

	const vector<Animation::Action>& Animation::GetActions() const {
		return actions;
	}

	void Animation::SetAnimationTexture(const string& textureID) {
		this->textureID = textureID;
	}

	const string& Animation::GetAnimationTexture() const {
		return textureID;
	}

	void Animation::SetCallback(const function<void()>& callback) {
		// Accepts nullptr. Callback executions will be ignored if nullptr, though.
		this->callback = callback;
	}

	void Animation::ExecCallback() const {
		if (callback != nullptr) callback();
	}
}