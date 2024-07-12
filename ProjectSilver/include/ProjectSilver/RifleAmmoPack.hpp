#pragma once

#include <ProjectSilver/Actor.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    class RifleAmmoPack : public Actor
    {
        int pickUpDelay;
        int ammoToGrant;

    public:
        inline void SetAmmoToGrant(int ammo)
        {
            this->ammoToGrant = ammo;
        }

        RifleAmmoPack();
        ~RifleAmmoPack();

        virtual void Update();
    };
} // namespace Game
