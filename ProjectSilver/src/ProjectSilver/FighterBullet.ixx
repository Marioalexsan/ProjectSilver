module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.FighterBullet;
import ProjectSilver.Actor;

export namespace Game
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
