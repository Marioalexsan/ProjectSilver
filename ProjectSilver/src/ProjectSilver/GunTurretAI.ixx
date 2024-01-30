module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.GunTurretAI;
import ProjectSilver.AI;

export namespace Game
{
    class GunTurretAI : public AI
    {
    private:
        int  nextShot;
        int  previousShot;
        bool waveBonus;
        int  ammo;
        int  reloadCounter;
        int  burstDelay;

        bool initiatedAttack;

        double predictionStrengthToUse;

        Vector2 lastFramePlayerPos;

    public:
        GunTurretAI();

        void ApplyWaveEndBonus();

        virtual void OnAttackHit();
        virtual void OnHitByAttack(Actor* attacker, double damage);

        virtual void Update();

        virtual void OnDeath();
    };
} // namespace Game
