#ifndef COLLISION_MASTER
#define COLLISION_MASTER

#include "PCHeader.h"
#include "BoxCollider.h"
#include "SphereCollider.h"

namespace Game {
	namespace CollisionMaster {
		pair<bool, Vector2> CheckCollision(const BoxCollider& A, const BoxCollider& B);
		pair<bool, Vector2> CheckCollision(const BoxCollider& A, const SphereCollider& B);

		pair<bool, Vector2> CheckCollision(const SphereCollider& A, const SphereCollider& B);
		pair<bool, Vector2> CheckCollision(const SphereCollider& A, const BoxCollider& B);
	}
}

#endif