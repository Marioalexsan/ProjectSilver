#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/ShadowHPBar.hpp>

namespace Game
{
    ShadowHPBar::ShadowHPBar() :
    HPtex(nullptr),
    lastStandHPtex(nullptr),
    HPValue(0),
    lastStandHPValue(0),
    maxHP(0)
    {
    }

    void ShadowHPBar::SetHPTexture(const std::string& ID)
    {
        auto& lib = Globals::Assets().GetTextureLibrary();
        if (lib.find(ID) == lib.end())
        {
            //Error
            return;
        }
        this->HPtex = lib.at(ID).texture.get();
    }

    void ShadowHPBar::SetLastStandHPTexture(const std::string& ID)
    {
        auto& lib = Globals::Assets().GetTextureLibrary();
        if (lib.find(ID) == lib.end())
        {
            //Error
            return;
        }
        this->lastStandHPtex = lib.at(ID).texture.get();
    }

    void ShadowHPBar::SetHPValues(double HP, double lastStandHP, double maxHP)
    {
        HPValue          = HP;
        lastStandHPValue = lastStandHP;
        this->maxHP      = maxHP;
    }

    void ShadowHPBar::Draw()
    {
        using namespace Game::Utility;
        if (texture == nullptr || HPtex == nullptr || lastStandHPtex == nullptr)
        {
            return;
        }

        double HPRatio          = 0.0;
        double LastStandHPRatio = 0.0;
        if (maxHP > 0.0)
        {
            HPRatio          = abs(HPValue / maxHP);
            LastStandHPRatio = abs(lastStandHPValue / maxHP);
        }

        // Border
        {
            sf::Sprite sprite(*texture);

            sprite.setPosition(
                sf::Vector2f{(float)transform->position.x, (float)transform->position.y});
            sprite.setOrigin(
                sf::Vector2f{(float)transform->center.x, (float)transform->center.y});
            sprite.setColor(sf::Color{color.r, color.g, color.b, alpha});
            sprite.setRotation(sf::degrees(transform->direction));

            GraphicsEngine::World->draw(sprite);
        }

        // Last Stand HP
        {
            sf::Sprite sprite(*lastStandHPtex);

            sprite.setPosition(
                sf::Vector2f{(float)transform->position.x - (float)transform->center.x + 2.f,
                             (float)transform->position.y - (float)transform->center.y + 2});
            sprite.setScale({float((texture->getSize().x - 4) * LastStandHPRatio), 1});
            sprite.setColor(sf::Color{color.r, color.g, color.b, alpha});
            sprite.setRotation(sf::degrees(transform->direction));

            GraphicsEngine::World->draw(sprite);
        }

        // HP
        {
            sf::Sprite sprite(*HPtex);

            sprite.setPosition(
                sf::Vector2f{(float)transform->position.x - (float)transform->center.x + 2.f,
                             (float)transform->position.y - (float)transform->center.y + 2});
            sprite.setScale({float((texture->getSize().x - 4) * HPRatio), 1});
            sprite.setColor(sf::Color{color.r, color.g, color.b, alpha});
            sprite.setRotation(sf::degrees(transform->direction));

            GraphicsEngine::World->draw(sprite);
        }
    }
} // namespace Game