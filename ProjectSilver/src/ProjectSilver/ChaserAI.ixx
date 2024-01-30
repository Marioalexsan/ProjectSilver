module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.ChaserAI;
import ProjectSilver.SphereCollider;
import ProjectSilver.AI;

export namespace Game
{
    class ChaserAI : public AI
    {
    private:
        bool strafesLeft;
        int  nextStrafeChange;
        bool doingSwing;

        int postSwingDelay;
        int painCounter;

        double currentSpeed;
        int    fastSwingCounter;

        SphereCollider axe;

        Vector2 lastFramePlayerPos;

        int specialHurtSoundCooldown;

    public:
        ChaserAI();

        virtual void OnAttackHit();
        virtual void OnHitByAttack(Actor* attacker, double damage);

        virtual void Update();

        virtual void OnDeath();
    };
} // namespace Game
