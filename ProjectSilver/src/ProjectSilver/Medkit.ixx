module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.Medkit;
import ProjectSilver.Actor;

export namespace Game
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