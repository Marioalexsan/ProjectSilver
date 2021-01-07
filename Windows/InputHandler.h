#ifndef INPUT_HEADER
#define INPUT_HEADER

#include "PCHeader.h"

namespace Game {
	class InputHandler {
	public:
		enum ButtonCode {
			NoButton = -1,
			Left = 0,
			Middle = 1,
			Right = 2
		};

		enum KeyCode {
			NoKey = -1,
			A, B, C, D, E, F, G, H, I,
			J, K, L, M, N, O, P, Q, R,
			S, T, U, V, W, X, Y, Z,
			LShift
		};

		static const map<SDL_Scancode, KeyCode> keyTranslation;

		static const int framesToRemember = 2;

	private:
		const uint8_t* keyArray;
		int keyCount;

		bool gameFocused;
		bool quitCalled;

		int virtualMouseX;
		int virtualMouseY;

		map<ButtonCode, bool> ButtonFrames[framesToRemember];
		map<KeyCode, bool> KeyFrames[framesToRemember];

		void ProcessEvents();
		void ProcessInputs();
		void PushFrame();
	public:
		InputHandler();

		void SetMouseGrab(bool grab);

		void Update();
		pair<int, int> GetMousePosition();
		pair<int, int> GetRelativeMousePosition();

		bool IsKeyDown(KeyCode code);
		bool IsButtonDown(ButtonCode code);
		bool IsKeyPressedThisFrame(KeyCode code);
		bool IsButtonPressedThisFrame(ButtonCode code);

		bool WasQuitCalled();
	};
}

#endif