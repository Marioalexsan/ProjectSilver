module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.RifleAmmoPack;
import ProjectSilver.Actor;

export namespace Game
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
