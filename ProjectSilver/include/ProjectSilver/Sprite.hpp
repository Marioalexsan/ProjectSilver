#pragma once

#include <ProjectSilver/Drawable.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    class Sprite : public Drawable
    {
    protected:
        sf::Texture* texture;

    public:
        Sprite();

        virtual void SetTexture(const std::string& ID);
        Vector2      GetSize();

        virtual void Draw();
    };
} // namespace Game
