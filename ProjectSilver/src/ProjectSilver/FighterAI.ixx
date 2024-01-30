module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.FighterAI;
import ProjectSilver.AI;

export namespace Game
{
    class FighterAI : public AI
    {
    private:
        Vector2 lastFramePlayerPos;

        double predictionStrengthToUse;

        int nextStrafeChange;
        int nextShot;
        int previousShot;
        int aimAnnoyance;

        bool strafesLeft;

        bool seesPlayer;

    public:
        FighterAI();

        virtual void OnAttackHit();
        virtual void OnHitByAttack(Actor* attacker, double damage);

        virtual void Update();

        virtual void OnDeath();
    };
} // namespace Game
