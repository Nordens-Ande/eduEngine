#pragma once

#include "Components.hpp"

namespace collision
{
	class BVH
	{
	public:
		using Sphere = ecs::SphereColliderComponent;

		float DistanceBetweenSpheres(Sphere* leftSphere, Sphere* rightSphere);
		void FindMinMaxPoints(glm::vec3 leftCenter, glm::vec3 rightCenter, float leftRadius, float rightRadius, glm::vec3& minOut, glm::vec3& maxOut);

		struct SphereNode
		{
			Sphere* collisionRepresentation;
			SphereNode* leftChild;
			SphereNode* rightChild;
		};
		SphereNode* BuildNodeFromSingleSphere(Sphere* sphere);
		SphereNode* BuildNodeFromSpheres(Sphere* leftSphere, Sphere* rightSphere);

		std::vector<std::pair<SphereNode*, SphereNode*>> FindPairs(std::vector<SphereNode*> openList, float maxDistance);
		SphereNode* BuildBVHBottomUp(std::vector<Sphere*> spheres, float maxDistanceBetweenLeaves);
		std::vector<Sphere*> FindPossibleCollisions(SphereNode* treeRoot, Sphere* sphere);
	};
}


