#pragma once

#include <ProjectSilver/Collider.hpp>
#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/SphereCollider.hpp>

namespace Game
{
    class BoxCollider : public Collider
    {
    protected:
        double width;
        double height;

        // Position indicates top left corner

    public:
        BoxCollider();
        BoxCollider(const Vector2& topLeft, double width, double height, ColliderType type);

        virtual const Vector2& GetPosition() const;
        Vector2                GetCenteredPosition() const;

        void                      SetSize(double width, double height);
        std::pair<double, double> GetSize() const;

        void SetCenteredPosition(const Vector2& position);

        virtual std::pair<Vector2, Vector2> GetBoundingBox();
    };
} // namespace Game
