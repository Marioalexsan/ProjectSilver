#pragma once

#include <ProjectSilver/Collider.hpp>
#include <ProjectSilver/PCHeader.hpp>

namespace Game
{
    class SphereCollider : public Collider
    {
    protected:
        double radius;

    public:
        SphereCollider();
        SphereCollider(const Vector2& center, double radius, ColliderType type);

        void   SetRadius(double radius);
        double GetRadius() const;

        virtual std::pair<Vector2, Vector2> GetBoundingBox();
    };
} // namespace Game
