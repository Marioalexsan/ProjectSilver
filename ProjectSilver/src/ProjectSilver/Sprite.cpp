#include <ProjectSilver/Globals.hpp>
#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/Sprite.hpp>

namespace Game
{
    Sprite::Sprite() : texture(nullptr)
    {
    }

    void Sprite::SetTexture(const std::string& ID)
    {
        auto& lib = Globals::Assets().GetTextureLibrary();
        if (lib.find(ID) == lib.end())
        {
            //Error
            return;
        }
        this->texture = lib.at(ID).texture.get();
    }

    Vector2 Sprite::GetSize()
    {
        auto size = texture->getSize();
        return {(double)size.x, (double)size.y};
    }

    void Sprite::Draw()
    {
        using namespace Game::Utility;
        if (texture == nullptr)
        {
            return;
        }

        sf::Sprite sprite(*texture);

        sprite.setPosition(
            sf::Vector2f{(float)transform->position.x, (float)transform->position.y});
        sprite.setOrigin(sf::Vector2f{(float)transform->center.x, (float)transform->center.y});
        sprite.setColor(sf::Color{color.r, color.g, color.b, alpha});
        sprite.setRotation(sf::degrees(transform->direction));
        sprite.setScale({(float)scale.x, (float)scale.y});

        GraphicsEngine::World->draw(sprite);
    }


} // namespace Game