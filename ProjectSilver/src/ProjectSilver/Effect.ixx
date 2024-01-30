module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.Effect;
import ProjectSilver.Drawable;

export namespace Game
{
    // Effects - drawables that are temporary in nature, and provide extra visuals
    // Effects are usually "created and abandoned"
    // All effects have a set amount of time before they expire and are destroyed
    // GameMaster can also forcefully purge all effects

    // Effects also have an Update() method
    // Effects are likely to ignore Center, Position and Direction elements from Drawables

    enum class EffectType
    {
        Unknown,
        BulletTracer
    };

    class Effect : public Drawable
    {
    protected:
        int timeToLive;

    public:
        Effect();

        virtual void Update(); // Must either be called by dervied classes, or timeToLive needs to be counted down manually

        inline int GetTimeLeft()
        {
            return timeToLive;
        }
        inline void SetTimeLeft(int frames)
        {
            timeToLive = frames;
        }
    };
} // namespace Game
