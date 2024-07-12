#pragma once

#include <ProjectSilver/Actor.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    class FighterBullet : public Actor
    {
        double distancePerFrame;
        double currentDistance;
        double maxDistance;

    public:
        FighterBullet();
        ~FighterBullet();

        virtual void Update();
    };
} // namespace Game
