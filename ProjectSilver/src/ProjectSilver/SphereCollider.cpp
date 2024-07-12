#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/SphereCollider.hpp>

namespace Game
{
    SphereCollider::SphereCollider() : Collider(Vector2::Zero, Static), radius(10.0)
    {
    }

    SphereCollider::SphereCollider(const Vector2& center, double radius, ColliderType type) :
    Collider(center, type),
    radius(abs(radius))
    {
    }

    void SphereCollider::SetRadius(double radius)
    {
        this->radius = abs(radius);
    }

    double SphereCollider::GetRadius() const
    {
        return radius;
    }

    std::pair<Vector2, Vector2> SphereCollider::GetBoundingBox()
    {
        auto& pos = GetPosition();
        return {Vector2(pos.x - radius, pos.y - radius),
                Vector2(pos.x + radius, pos.y + radius)};
    }
} // namespace Game