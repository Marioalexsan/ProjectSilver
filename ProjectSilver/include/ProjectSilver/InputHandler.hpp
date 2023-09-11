#pragma once

#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    class InputHandler
    {
    public:
        enum ButtonCode
        {
            NoButton = -1,
            Left     = 0,
            Middle   = 1,
            Right    = 2
        };

        enum KeyCode
        {
            NoKey = -1,
            A,
            B,
            C,
            D,
            E,
            F,
            G,
            H,
            I,
            J,
            K,
            L,
            M,
            N,
            O,
            P,
            Q,
            R,
            S,
            T,
            U,
            V,
            W,
            X,
            Y,
            Z,
            NumOne,
            NumTwo,
            LShift
        };

        static const std::map<sf::Keyboard::Key, KeyCode> keyTranslation;
        static const std::map<sf::Mouse::Button, ButtonCode> buttonTranslation;

        static const int framesToRemember = 2;

    private:
        bool gameFocused;
        bool quitCalled;

        int virtualMouseX;
        int virtualMouseY;

        std::map<ButtonCode, bool> LastFrameButtons;
        std::map<KeyCode, bool>    LastFrameKeys;

        std::map<ButtonCode, bool> ThisFrameButtons;
        std::map<KeyCode, bool>    ThisFrameKeys;

        void ProcessEvents();
        void ProcessInputs();
        void PushFrame();

    public:
        InputHandler();

        void SetMouseGrab(bool grab);

        void                Update();
        std::pair<int, int> GetMousePosition();
        std::pair<int, int> GetRelativeMousePosition();

        bool IsKeyDown(KeyCode code);
        bool IsButtonDown(ButtonCode code);
        bool IsKeyPressedThisFrame(KeyCode code);
        bool IsButtonPressedThisFrame(ButtonCode code);

        bool WasQuitCalled();
    };
} // namespace Game
