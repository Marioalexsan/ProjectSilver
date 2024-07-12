#pragma once

#include <ProjectSilver/BasicText.hpp>
#include <ProjectSilver/BoxCollider.hpp>
#include <ProjectSilver/Entity.hpp>
#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/SphereCollider.hpp>
#include <ProjectSilver/Sprite.hpp>

namespace Game
{
    class MenuDirector : public Entity
    {
    public:
        struct BasicButton
        {
            Game::BoxCollider col;
            Game::Sprite      sprite;
            Game::BasicText   text;
        };

        struct ValueButton
        {
            Game::BoxCollider subCol;
            Game::BoxCollider addCol;
            Game::Sprite      sprite;
            Game::BasicText   text;
        };

        enum MenuState
        {
            MainScreen,
            Options
        };

    private:
        Game::BasicText gameTitle;

        // ... Main Screen
        // Play
        BasicButton mainPlay;
        BasicButton mainDiff;
        BasicText   mainDiffExtra;
        BasicButton mainOptions;
        BasicButton mainQuit;
        BasicText   mainTip;

        BasicButton optionsRes;
        BasicText   optionsResExtra;
        BasicButton optionsVideo;
        BasicText   optionsVideoExtra;
        BasicButton optionsMain;

        ValueButton optionsMVol; // Music
        BasicText   optionsMVolExtra;
        ValueButton optionsSVol; // Sound
        BasicText   optionsSVolExtra;

        MenuState currentState;

        int currentVideoMode;

        void StateEnter(MenuState state);
        void StateExitCurrent();

        void ChangeDifficulty();
        void ChangeResolution(int nextVideoMode);
        void ChangeVideoMode();

        void CycleTip();

    public:
        MenuDirector();
        ~MenuDirector();


        virtual void Update();
    };
} // namespace Game
