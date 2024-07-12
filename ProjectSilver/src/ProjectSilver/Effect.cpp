#include <ProjectSilver/Effect.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    Effect::Effect() : timeToLive(0)
    {
    }

    void Effect::Update()
    {
        if (timeToLive > 0)
        {
            timeToLive--;
        }
    }
} // namespace Game