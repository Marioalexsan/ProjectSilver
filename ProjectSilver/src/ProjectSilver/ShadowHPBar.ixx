module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.ShadowHPBar;
import ProjectSilver.Sprite;

export namespace Game
{
    class ShadowHPBar : public Sprite
    {
    protected:
        sf::Texture* lastStandHPtex;
        sf::Texture* HPtex;

        double HPValue;
        double lastStandHPValue;
        double maxHP;

    public:
        ShadowHPBar();

        void SetHPTexture(const std::string& texture);
        void SetLastStandHPTexture(const std::string& texture);

        void SetHPValues(double HP, double lastStandHP, double maxHP);

        virtual void Draw();
    };
} // namespace Game
