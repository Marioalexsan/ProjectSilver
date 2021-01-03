#include "PCHeader.h"
#include "CollisionMaster.h"

namespace Game {
	namespace CollisionMaster {
		// Returns if a collision exists, and the smallest displacement that would push box 2 out of box 1
		pair<bool, Vector2> CheckCollision(const BoxCollider& A, const BoxCollider& B, bool checkOnly) {
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
				if (checkOnly) {
					return { true, Vector2::Zero };
				}
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

		pair<bool, Vector2> CheckCollision(const BoxCollider& A, const SphereCollider& B, bool checkOnly) {
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
				if (checkOnly) {
					return { true, Vector2::Zero };
				}

				// Push sphere by shortest axis distance because I'm too cheap
				// Manipulate closest# values to create push vector
				auto pushVec = BPos - Vector2(closestX, closestY);
				auto len = pushVec.Length();
				pushVec = Vector2::NormalVector(pushVec.Angle()) * (BRad - len);
				return { true, pushVec };
			}
			
			return { false, Vector2::Zero };
		}

		pair<bool, Vector2> CheckCollision(const SphereCollider& A, const SphereCollider& B, bool checkOnly) {
			auto LenVector = B.GetPosition() - A.GetPosition();
			auto concatRads = A.GetRadius() + B.GetRadius();
			auto len = LenVector.Length();
			auto diff = concatRads - len;

			// Check if the distance from center 1 to center 2 is lower than radius sum
			if (diff > 0.0) {
				if (checkOnly) {
					return { true, Vector2::Zero };
				}

				// Calculate push for sphere 2
				auto normal = Vector2::NormalVector(LenVector.Angle());
				return { true, normal * diff * 1.01 }; // 1.01 is overcompensation - eliminates math errors that make spheres still touch
			}

			return { false, Vector2::Zero };
		}

		pair<bool, Vector2> CheckCollision(const SphereCollider& A, const BoxCollider& B, bool checkOnly) {
			// Just calls the other version and reverses push vector
			auto var = CheckCollision(B, A);
			return { var.first, var.second * -1 };
		}

		pair<bool, double> RaycastVSBox(const Vector2& A, const Vector2& B, const BoxCollider& Box) {
			// Algorithm taken (and modified) from https://noonat.github.io/intersect/#aabb-vs-segment

			double scaleX = 1.0 / (B.x - A.x);
			double scaleY = 1.0 / (B.y - A.y);
			double signX = scaleX > 0.0 ? 1.0 : -1.0;
			double signY = scaleY > 0.0 ? 1.0 : -1.0;

			auto pos = Box.GetCenteredPosition();
			auto size = Box.GetSize();

			double nearTimeX = (pos.x - signX * size.first / 2.0 - A.x) * scaleX;
			double nearTimeY = (pos.y - signY * size.second / 2.0 - A.y) * scaleY;
			double farTimeX = (pos.x + signX * size.first / 2.0 - A.x) * scaleX;
			double farTimeY = (pos.y + signY * size.second / 2.0 - A.y) * scaleY;

			if (nearTimeX > farTimeY || nearTimeY > farTimeX) {
				return { false, 0.0 };
			}

			double nearTime = nearTimeX > nearTimeY ? nearTimeX : nearTimeY;
			double farTime = farTimeX < farTimeY ? farTimeX : farTimeY;

			if (nearTime >= 1 || farTime <= 0) {
				return { false, 0.0 };
			}

			// I only need to know hit time and position

			double hitTime = Utility::ClampValue(nearTime, 0.0, 1.0);

			return { true, hitTime };
		}

		pair<bool, double> RaycastVSSphere(const Vector2& A, const Vector2& B, const SphereCollider& Sphere) {
			// This one is also taken and modified from https://stackoverflow.com/questions/1073336/circle-line-segment-collision-detection-algorithm

			Vector2 d = B - A;
			Vector2 f = A - Sphere.GetPosition();
			double r = Sphere.GetRadius();

			double a = d.x * d.x + d.y * d.y;
			double b = 2 * (f.x * d.x + f.y * d.y);
			double c = f.x * f.x + f.y * f.y - r * r;

			double discriminant = b * b - 4 * a * c;
			if (discriminant < 0)
			{
				// no intersection
				return { false, 0.0 };
			}
			else
			{
				// ray didn't totally miss sphere,
				// so there is a solution to
				// the equation.

				discriminant = sqrt(discriminant);

				// either solution may be on or off the ray so need to test both
				// t1 is always the smaller value, because BOTH discriminant and
				// a are nonnegative.
				double t1 = (-b - discriminant) / (2 * a);
				double t2 = (-b + discriminant) / (2 * a);

				if (t1 >= 0 && t1 <= 1)
				{
					return { true, t1 };
				}

				// here t1 didn't intersect so we are either started
				// inside the sphere or completely past it
				if (t2 >= 0 && t2 <= 1)
				{
					// ExitWound
					return { true, t2 };
				}

				// no intn: FallShort, Past, CompletelyInside
				return { false, 0.0 };
			}
		}
	}
}