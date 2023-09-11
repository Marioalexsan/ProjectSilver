#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/GraphicsEngine.hpp>
#include <ProjectSilver/InputHandler.hpp>
#include <ProjectSilver/MiscUtility.hpp>
#include <ProjectSilver/PCHeader.hpp>
#include <numeric>

using SFKey    = sf::Keyboard::Key;
using SFButton = sf::Mouse::Button;

namespace Game
{
    const std::map<sf::Keyboard::Key, InputHandler::KeyCode> InputHandler::keyTranslation =
        {{SFKey::A, A},         {SFKey::B, B},        {SFKey::C, C},
         {SFKey::D, D},         {SFKey::E, E},        {SFKey::F, F},
         {SFKey::G, G},         {SFKey::H, H},        {SFKey::I, I},
         {SFKey::J, J},         {SFKey::K, K},        {SFKey::L, L},
         {SFKey::M, M},         {SFKey::N, N},        {SFKey::O, O},
         {SFKey::P, P},         {SFKey::Q, Q},        {SFKey::R, R},
         {SFKey::S, S},         {SFKey::T, T},        {SFKey::U, U},
         {SFKey::V, V},         {SFKey::W, W},        {SFKey::X, X},
         {SFKey::Y, Y},         {SFKey::Z, Z},        {SFKey::LShift, LShift},
         {SFKey::Num1, NumOne}, {SFKey::Num2, NumTwo}};

    const std::map<sf::Mouse::Button, InputHandler::ButtonCode> InputHandler::buttonTranslation =
        {{SFButton::Left, Left}, {SFButton::Right, Right}, {SFButton::Middle, Middle}};

    InputHandler::InputHandler() :
    gameFocused(false),
    quitCalled(false),
    virtualMouseX(0),
    virtualMouseY(0)
    {
    }

    // "World" position
    std::pair<int, int> InputHandler::GetMousePosition()
    {
        return {virtualMouseX, virtualMouseY};
    }

    void InputHandler::SetMouseGrab(bool grab)
    {
        GraphicsEngine::Window->setMouseCursorGrabbed(grab);
    }

    void InputHandler::PushFrame()
    {
        for (auto& [key, state] : ThisFrameButtons)
            LastFrameButtons[key] = state;

        for (auto& [key, state] : ThisFrameKeys)
            LastFrameKeys[key] = state;
    }

    void InputHandler::Update()
    {
        PushFrame();
        ProcessEvents();
    }

    void InputHandler::ProcessEvents()
    {
        sf::Event event;
        while (GraphicsEngine::Window->pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::KeyPressed:
                {
                    if (keyTranslation.contains(event.key.code))
                        ThisFrameKeys[keyTranslation.at(event.key.code)] = true;
                }
                break;

                case sf::Event::KeyReleased:
                {
                    if (keyTranslation.contains(event.key.code))
                        ThisFrameKeys[keyTranslation.at(event.key.code)] = false;
                }
                break;

                case sf::Event::MouseButtonPressed:
                {
                    if (buttonTranslation.contains(event.mouseButton.button))
                        ThisFrameButtons[buttonTranslation.at(event.mouseButton.button)] = true;
                }
                break;

                case sf::Event::MouseButtonReleased:
                {
                    if (buttonTranslation.contains(event.mouseButton.button))
                        ThisFrameButtons[buttonTranslation.at(event.mouseButton.button)] = false;
                }
                break;

                case sf::Event::MouseMoved:
                {
                    // Resolution dependent sensitivity component
                    auto res = Globals::Graphics().GetWindowSize();

                    virtualMouseX = Utility::ClampValue(int(event.mouseMove.x / res.x * 1920.0),
                                                        0,
                                                        1920);
                    virtualMouseY = Utility::ClampValue(int(event.mouseMove.y / res.y * 1080.0),
                                                        0,
                                                        1080);
                }
                break;


                    // Misc

                case sf::Event::GainedFocus:
                {
                    SetMouseGrab(true);
                    gameFocused = true;
                }
                break;

                case sf::Event::LostFocus:
                {
                    SetMouseGrab(false);
                    gameFocused = false;
                }
                break;

                case sf::Event::Closed:
                {
                    quitCalled = true;
                }
                break;
            }
        }
    }


    bool InputHandler::IsKeyDown(KeyCode code)
    {
        return ThisFrameKeys[code];
    }

    bool InputHandler::IsButtonDown(ButtonCode code)
    {
        return ThisFrameButtons[code];
    }

    bool InputHandler::IsKeyPressedThisFrame(KeyCode code)
    {
        return ThisFrameKeys[code] && !LastFrameKeys[code];
    }

    bool InputHandler::IsButtonPressedThisFrame(ButtonCode code)
    {
        return ThisFrameButtons[code] && !LastFrameButtons[code];
    }

    bool InputHandler::WasQuitCalled()
    {
        return quitCalled;
    }
} // namespace Game