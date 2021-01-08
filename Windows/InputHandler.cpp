#include "PCHeader.h"

#include "InputHandler.h"
#include "MiscUtility.h"
#include "GraphicsEngine.h"

namespace Game {
	const map<SDL_Scancode, InputHandler::KeyCode> InputHandler::keyTranslation = {
			{SDL_SCANCODE_A, A}, {SDL_SCANCODE_B, B}, {SDL_SCANCODE_C, C}, {SDL_SCANCODE_D, D},
			{SDL_SCANCODE_E, E}, {SDL_SCANCODE_F, F}, {SDL_SCANCODE_G, G}, {SDL_SCANCODE_H, H},
			{SDL_SCANCODE_I, I}, {SDL_SCANCODE_J, J}, {SDL_SCANCODE_K, K}, {SDL_SCANCODE_L, L},
			{SDL_SCANCODE_M, M}, {SDL_SCANCODE_N, N}, {SDL_SCANCODE_O, O}, {SDL_SCANCODE_P, P},
			{SDL_SCANCODE_Q, Q}, {SDL_SCANCODE_R, R}, {SDL_SCANCODE_S, S}, {SDL_SCANCODE_T, T},
			{SDL_SCANCODE_U, U}, {SDL_SCANCODE_V, V}, {SDL_SCANCODE_W, W}, {SDL_SCANCODE_X, X},
			{SDL_SCANCODE_Y, Y}, {SDL_SCANCODE_Z, Z}, {SDL_SCANCODE_1, NumOne}, {SDL_SCANCODE_2, NumTwo}
	};

	InputHandler::InputHandler():
		gameFocused(false),
		quitCalled(false),
		virtualMouseX(0),
		virtualMouseY(0),
		keyArray(SDL_GetKeyboardState(&keyCount))
	{
		ButtonCode arr[] = { Left, Middle, Right };
		KeyCode morearr[] = { A, D, S, W, E, LShift };
		for (auto elem : arr) {
			for (int i = 0; i < framesToRemember; i++) {
				ButtonFrames[i][elem] = false;
			}
		}
		for (auto elem : morearr) {
			for (int i = 0; i < framesToRemember; i++) {
				KeyFrames[i][elem] = false;
			}
		}
	}

	// "World" position
	pair<int, int> InputHandler::GetMousePosition() {
		return { virtualMouseX, virtualMouseY };
	}

	// "True" positon
	pair<int, int> InputHandler::GetRelativeMousePosition() {
		auto var = GraphicsEngine::GetWindowSize();
		return { virtualMouseX * var.x / 1920, virtualMouseY * var.y / 1080};
	}

	void InputHandler::SetMouseGrab(bool grab) {
		SDL_SetRelativeMouseMode((SDL_bool)grab);
	}

	void InputHandler::PushFrame() {
		for (int i = framesToRemember - 1; i > 0; i--) {
			ButtonFrames[i] = ButtonFrames[i - 1];
		}
		for (auto& button : ButtonFrames[0]) button.second = false;


		for (int i = framesToRemember - 1; i > 0; i--) {
			KeyFrames[i] = KeyFrames[i - 1];
		}
		for (auto& key : KeyFrames[0]) key.second = false;
	}

	void InputHandler::ProcessInputs() {
		for (auto& elem : keyTranslation) {
			KeyFrames[0][elem.second] = keyArray[elem.first];
		}
		KeyFrames[0][KeyCode::LShift] = SDL_GetModState() & KMOD_LSHIFT;
		auto mouseState = SDL_GetMouseState(nullptr, nullptr);
		ButtonFrames[0][ButtonCode::Left] = bool(mouseState & SDL_BUTTON(SDL_BUTTON_LEFT));
		ButtonFrames[0][ButtonCode::Middle] = bool(mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE));
		ButtonFrames[0][ButtonCode::Right] = bool(mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT));
	}

	void InputHandler::Update() {
		PushFrame();
		ProcessEvents();
		ProcessInputs();
	}



	void InputHandler::ProcessEvents() {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {

				// Mouse

			case SDL_EventType::SDL_MOUSEMOTION:
				// Limited to game area
				virtualMouseX = Utility::ClampValue(virtualMouseX + event.motion.xrel, 0, 1920);
				virtualMouseY = Utility::ClampValue(virtualMouseY + event.motion.yrel, 0, 1080);
				break;

				// Misc

			case SDL_EventType::SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					gameFocused = true;
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					gameFocused = false;
					break;
				case SDL_QUIT:
				case SDL_WINDOWEVENT_CLOSE:
					quitCalled = true;
					break;
				}
				break;
			}
		}
	}


	bool InputHandler::IsKeyDown(KeyCode code) {
		return KeyFrames[0][code];
	}

	bool InputHandler::IsButtonDown(ButtonCode code) {
		return ButtonFrames[0][code];
	}

	bool InputHandler::IsKeyPressedThisFrame(KeyCode code) {
		return KeyFrames[0][code] && !KeyFrames[1][code];
	}

	bool InputHandler::IsButtonPressedThisFrame(ButtonCode code) {
		return ButtonFrames[0][code] && !ButtonFrames[1][code];
	}

	bool InputHandler::WasQuitCalled() {
		return quitCalled;
	}
}