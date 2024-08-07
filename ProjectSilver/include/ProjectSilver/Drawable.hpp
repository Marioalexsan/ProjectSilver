#pragma once

#include <ProjectSilver/MiscUtility.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{

    struct Color
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;

        static const Color White;
        static const Color Black;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Yellow;
        static const Color Orange;
        static const Color Purple;
        static const Color Aqua;
        static const Color Gray;
    };

    // Base interface for drawable
    // Angle is from OY+ axis, clockwise

    class Drawable
    {
    protected:
        Trackable<Game::Transform> transform;

        int     layer;
        bool    relativeToCamera;
        Vector2 scale = Vector2(1, 1);

        uint8_t alpha;
        Color   color;

        uint64_t drawableGraphicsID = 0;

    public:
        Drawable();
        virtual ~Drawable();
        virtual void Draw() = 0;

        Trackable<Game::Transform>& GetTransform();

        void SetDirection(double direction);
        void Rotate(double angle);

        inline void SetAlpha(uint8_t alpha)
        {
            this->alpha = alpha;
        }

        inline uint8_t GetAlpha()
        {
            return alpha;
        }

        /** Only implemented for BasicSprite ATM */
        inline void SetScale(Vector2 scale)
        {
            this->scale = scale;
        }

        /** Only implemented for BasicSprite ATM */
        inline Vector2 GetScale()
        {
            return scale;
        }

        inline void SetColor(Color color)
        {
            this->color = color;
        }

        inline Color GetColor()
        {
            return color;
        }

        void SetPosition(const Vector2& position);
        void Move(const Vector2& amount);

        void SetCenter(const Vector2& center);

        int  GetLayer();
        void SetLayer(int layer);

        bool GetRelativeToCamera();
        void SetRelativeToCamera(bool relativity);

        void RegisterToGame();
        void UnregisterFromGame();
    };
} // namespace Game
