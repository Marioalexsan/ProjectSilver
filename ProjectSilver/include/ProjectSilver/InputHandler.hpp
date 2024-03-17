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

        enum class JoystickCode
        {
            A,
            B,
            X,
            Y,
            LeftBumper,
            RightBumper,
            Start,
            Back,
            LeftStickClick,
            RightStickClick
        };

        enum class AxisCode
        {
            LeftX,
            LeftY,
            RightX,
            RightY,
            Trigger,
        };

        static const std::map<sf::Keyboard::Key, KeyCode>    keyTranslation;
        static const std::map<sf::Mouse::Button, ButtonCode> buttonTranslation;
        static const std::map<unsigned int, JoystickCode>    joystickTranslation;
        static const std::map<sf::Joystick::Axis, AxisCode>  axisTranslation;

        static const int framesToRemember = 2;

    private:
        bool gameFocused;
        bool quitCalled;

        int virtualMouseX;
        int virtualMouseY;

        std::map<AxisCode, float> AxisValues;

        std::map<ButtonCode, bool>   LastFrameButtons;
        std::map<KeyCode, bool>      LastFrameKeys;
        std::map<JoystickCode, bool> LastFrameJoystick;

        std::map<ButtonCode, bool>   ThisFrameButtons;
        std::map<KeyCode, bool>      ThisFrameKeys;
        std::map<JoystickCode, bool> ThisFrameJoystick;

        void ProcessEvents();
        void ProcessInputs();
        void PushFrame();

    public:
        InputHandler();

        void SetMouseGrab(bool grab);

        void                Update();
        std::pair<int, int> GetMousePosition();
        std::pair<int, int> GetRelativeMousePosition();

        sf::Vector2f GetLeftAxis();
        sf::Vector2f GetRightAxis();

        float GetLeftTrigger();
        float GetRightTrigger();

        bool IsKeyDown(KeyCode code);
        bool IsButtonDown(ButtonCode code);
        bool IsJoystickDown(JoystickCode code);
        bool IsKeyPressedThisFrame(KeyCode code);
        bool IsButtonPressedThisFrame(ButtonCode code);
        bool IsJoystickPressedThisFrame(JoystickCode code);

        bool WasQuitCalled();
    };
} // namespace Game
