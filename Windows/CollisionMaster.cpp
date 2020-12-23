#include "PCHeader.h"
#include "CollisionMaster.h"

namespace Game {
	namespace CollisionMaster {
		// Returns if a collision exists, and the smallest displacement that would push box 2 out of box 1
		pair<bool, Vector2> CheckCollision(const BoxCollider& A, const BoxCollider& B) {
			// Position indicates top left corner
			// WH stands for WidthHeight
			auto AWH = A.GetSize();
			auto BWH = B.GetSize();
			auto APos = A.GetPosition();
			auto BPos = B.GetPosition();

			auto deltaOne = BPos.x + BWH.first - APos.x;
			auto deltaTwo = APos.x + AWH.first - BPos.x;
			auto deltaThree = BPos.y + BWH.second - APos.y;
			auto deltaFour = APos.y + AWH.second - BPos.y;

			// Check collision for AABB
			if (deltaOne > 0.0 && deltaTwo > 0.0 && deltaThree > 0.0 && deltaFour > 0.0)
			{
				double returnX = deltaOne < deltaTwo ? -deltaOne : deltaTwo;
				double returnY = deltaThree < deltaFour ? -deltaThree : deltaFour;
				if (abs(returnX) < abs(returnY)) {
					returnY = 0;
				}
				else {
					returnX = 0;
				}
				return { true, Vector2(returnX, returnY) };
			}
			return { false, Vector2::Zero };
		}

		pair<bool, Vector2> CheckCollision(const BoxCollider& A, const SphereCollider& B) {
			auto APos = A.GetPosition();
			auto BPos = B.GetPosition();
			auto AWH = A.GetSize();
			auto ALeft = APos.x;
			auto ARight = APos.x + AWH.first;
			auto ATop = APos.y;
			auto ABottom = APos.y + AWH.second;
			auto BRad = B.GetRadius();

			auto closestX = BPos.x;
			auto closestY = BPos.y;
			closestX = Utility::ClampValue(closestX, ALeft, ARight);
			closestY = Utility::ClampValue(closestY, ATop, ABottom);

			// Dheck if closest point is inside circle
			auto dist = (BPos - Vector2(closestX, closestY)).Length();
			if (dist < BRad) {
				// A Collision!!
				// Push sphere by shortest axis distance because I'm too cheap
				// Manipulate closest# values to create push vector
				auto pushVec = BPos - Vector2(closestX, closestY);
				auto len = pushVec.Length();
				pushVec = Vector2::NormalVector(pushVec.Angle()) * (BRad - len);
				return { true, pushVec };
			}
			
			return { false, Vector2::Zero };
		}

		pair<bool, Vector2> CheckCollision(const SphereCollider& A, const SphereCollider& B) {
			auto LenVector = B.GetPosition() - A.GetPosition();
			auto concatRads = A.GetRadius() + B.GetRadius();
			auto len = LenVector.Length();
			auto diff = concatRads - len;

			// Check if the distance from center 1 to center 2 is lower than radius sum
			if (diff > 0.0) {
				// Calculate push for sphere 2
				auto normal = Vector2::NormalVector(LenVector.Angle());
				return { true, normal * diff * 1.01 }; // 1.01 is overcompensation - eliminates math errors that make spheres still touch
			}

			return { false, Vector2::Zero };
		}

		pair<bool, Vector2> CheckCollision(const SphereCollider& A, const BoxCollider& B) {
			// Just calls the other version and reverses push vector
			auto var = CheckCollision(B, A);
			return { var.first, var.second * -1 };
		}
	}
}