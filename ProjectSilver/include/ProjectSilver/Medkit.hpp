#pragma once

#include <ProjectSilver/Actor.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    class Medkit : public Actor
    {
        int pickUpDelay;
        int healthToGrant;

    public:
        inline void SetHealthToGrant(int hp)
        {
            this->healthToGrant = hp;
        }

        Medkit();
        ~Medkit();

        virtual void Update();
    };
} // namespace Game
