module;

#include <ProjectSilver/PCHeader.hpp>

export module ProjectSilver.Sprite;
import ProjectSilver.Drawable;

export namespace Game
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
