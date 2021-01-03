#ifndef COLLISION_MASTER
#define COLLISION_MASTER

#include "PCHeader.h"
#include "BoxCollider.h"
#include "SphereCollider.h"

namespace Game {
	namespace CollisionMaster {
		pair<bool, Vector2> CheckCollision(const BoxCollider& A, const BoxCollider& B, bool checkOnly = false);
		pair<bool, Vector2> CheckCollision(const BoxCollider& A, const SphereCollider& B, bool checkOnly = false);

		pair<bool, Vector2> CheckCollision(const SphereCollider& A, const SphereCollider& B, bool checkOnly = false);
		pair<bool, Vector2> CheckCollision(const SphereCollider& A, const BoxCollider& B, bool checkOnly = false);
	
		pair<bool, double> RaycastVSBox(const Vector2& A, const Vector2& B, const BoxCollider& Box);
		pair<bool, double> RaycastVSSphere(const Vector2& A, const Vector2& B, const SphereCollider& Sphere);
	}
}

#endif