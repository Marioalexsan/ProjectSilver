#pragma once

#include <ProjectSilver/BoxCollider.hpp>
#include <ProjectSilver/PCHeader.hpp>
#include <ProjectSilver/SphereCollider.hpp>

namespace Game
{
    namespace CollisionMaster
    {
        std::pair<bool, Vector2> CheckCollision(const BoxCollider& A,
                                                const BoxCollider& B,
                                                bool               checkOnly = false);
        std::pair<bool, Vector2> CheckCollision(const BoxCollider&    A,
                                                const SphereCollider& B,
                                                bool                  checkOnly = false);

        std::pair<bool, Vector2> CheckCollision(const SphereCollider& A,
                                                const SphereCollider& B,
                                                bool                  checkOnly = false);
        std::pair<bool, Vector2> CheckCollision(const SphereCollider& A,
                                                const BoxCollider&    B,
                                                bool                  checkOnly = false);

        std::pair<bool, double> RaycastVSBox(const Vector2&     A,
                                             const Vector2&     B,
                                             const BoxCollider& Box);
        std::pair<bool, double> RaycastVSSphere(const Vector2&        A,
                                                const Vector2&        B,
                                                const SphereCollider& Sphere);

        bool PointCheckVSBox(const Vector2& point, const BoxCollider& Box);
    } // namespace CollisionMaster
} // namespace Game
