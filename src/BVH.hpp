#pragma once

//#include "Components.hpp"
#include <entt/entt.hpp>
#include "glmcommon.hpp"
#include <vector>

namespace ecs
{
	struct SphereComponent;
}

namespace collision
{
	class BVH
	{
	public:
		using Sphere = ecs::SphereComponent;
		entt::registry* registry;

		float DistanceBetweenSpheres(
			Sphere* leftSphere, 
			Sphere* rightSphere
		);
		void FindMinMaxPoints(
			glm::vec3 leftCenter, 
			glm::vec3 rightCenter, 
			float leftRadius, 
			float rightRadius, 
			glm::vec3& minOut, 
			glm::vec3& maxOut
		);

		bool SphereSphereTest(
			Sphere* a, 
			Sphere* b
		);
		struct SphereNode
		{
			entt::entity entity;
			Sphere* collisionRepresentation;
			SphereNode* leftChild;
			SphereNode* rightChild;
		};
		SphereNode* BuildNodeFromSingleSphere(
			Sphere* sphere
		);
		SphereNode* BuildNodeFromSingleSphere(
			entt::entity entity,
			Sphere* sphere
		);
		SphereNode* BuildNodeFromSpheres(
			Sphere* leftSphere, 
			Sphere* rightSphere
		);

		std::vector<std::pair<SphereNode*, SphereNode*>> FindPairs(
			std::vector<SphereNode*> openList, 
			float maxDistance
		);
		SphereNode* BuildBVHBottomUp(
			entt::registry& registry, 
			std::vector<entt::entity> entities, 
			float maxDistanceBetweenLeaves
		);
		std::vector<entt::entity> FindPossibleCollisions(
			SphereNode* treeRoot, 
			Sphere* sphere
		);
	};
}


