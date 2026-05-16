#include "BVH.hpp"
#include "Components.hpp"


namespace collision
{
    float BVH::DistanceBetweenSpheres(Sphere* leftSphere, Sphere* rightSphere)
    {
        float centerDistance = glm::length(rightSphere->position - leftSphere->position);
        float surfaceDistance = centerDistance - (leftSphere->radius + rightSphere->radius);
        return std::max(0.0f, surfaceDistance);
    }

    void BVH::FindMinMaxPoints(glm::vec3 leftCenter, glm::vec3 rightCenter, float leftRadius, float rightRadius, glm::vec3& minOut, glm::vec3& maxOut)
    {
        minOut.x = std::min(leftCenter.x - leftRadius, rightCenter.x - rightRadius);
        maxOut.x = std::max(leftCenter.x + leftRadius, rightCenter.x + rightRadius);

        minOut.y = std::min(leftCenter.y - leftRadius, rightCenter.y - rightRadius);
        maxOut.y = std::max(leftCenter.y + leftRadius, rightCenter.y + rightRadius);

        minOut.z = std::min(leftCenter.z - leftRadius, rightCenter.z - rightRadius);
        maxOut.z = std::max(leftCenter.z + leftRadius, rightCenter.z + rightRadius);
    }

    bool BVH::SphereSphereTest(Sphere* a, Sphere* b)
    {
        glm::vec3 centerToCenter = a->position - b->position;
        float distance = glm::dot(centerToCenter, centerToCenter);

        float radiusSum = a->radius + b->radius;
        return distance <= radiusSum * radiusSum;
    }

    BVH::SphereNode* BVH::BuildNodeFromSingleSphere(Sphere* sphere)
    {
        return new SphereNode{ entt::null, sphere, nullptr, nullptr };
    }
    BVH::SphereNode* BVH::BuildNodeFromSingleSphere(entt::entity entity, Sphere* sphere)
    {
        return new SphereNode{ entity, sphere, nullptr, nullptr };
    }

    BVH::SphereNode* BVH::BuildNodeFromSpheres(Sphere* leftSphere, Sphere* rightSphere)
    {
        glm::vec3 minPoint, maxPoint;
        FindMinMaxPoints(leftSphere->position, rightSphere->position, leftSphere->radius, rightSphere->radius, minPoint, maxPoint);

        glm::vec3 midPoint = minPoint + (maxPoint - minPoint) * 0.5f;
        float radius = glm::length(maxPoint - minPoint) * 0.5f;

        return new SphereNode{ entt::null, new Sphere{ false, false, midPoint, radius}, nullptr, nullptr };
    }

    std::vector<std::pair<BVH::SphereNode*, BVH::SphereNode*>> BVH::FindPairs(std::vector<SphereNode*> openList, float maxDistance)
    {
        std::vector<std::pair<SphereNode*, SphereNode*>> allPairs;
        std::vector<SphereNode*> availableSpheres = openList;

        while (!availableSpheres.empty()) {
            SphereNode* current = availableSpheres.back();
            availableSpheres.pop_back();

            float closestDistance = maxDistance;
            SphereNode* bestMatch = nullptr;
            int bestIndex = -1;

            for (int j = 0; j < availableSpheres.size(); ++j) {
                float distance = DistanceBetweenSpheres(
                    current->collisionRepresentation,
                    availableSpheres[j]->collisionRepresentation
                );

                if (distance < closestDistance) {
                    closestDistance = distance;
                    bestMatch = availableSpheres[j];
                    bestIndex = j;
                }
            }

            if (bestMatch) {
                availableSpheres.erase(
                    availableSpheres.begin() + bestIndex
                );
            }

            allPairs.push_back({ current, bestMatch });
        }

        return allPairs;
    }

    BVH::SphereNode* BVH::BuildBVHBottomUp(entt::registry& registry, std::vector<entt::entity> entities, float maxDistanceBetweenLeaves)
    {
        std::vector<Sphere*> spheres;
        BVH::registry = &registry;
        for (entt::entity entity : entities)
        {
            auto& sphere = BVH::registry->get<ecs::SphereComponent>(entity);
            spheres.push_back(&sphere);
        }

        std::vector<BVH::SphereNode*> openList;

        //for (Sphere* sphere : spheres)
        //{
        //    openList.push_back(BVH::BuildNodeFromSingleSphere(sphere));
        //}
        for (int i = 0; i < spheres.size(); i++)
        {
            openList.push_back(BVH::BuildNodeFromSingleSphere(entities[i], spheres[i]));
        }

        if (openList.empty())
            return nullptr;

        while (openList.size() > 1) {
            auto pairs = FindPairs(
                openList,
                maxDistanceBetweenLeaves
            );

            openList.clear();

            for (auto pair : pairs) {
                if (pair.second) {
                    auto node = BuildNodeFromSpheres(
                        pair.first->collisionRepresentation,
                        pair.second->collisionRepresentation
                    );

                    node->leftChild = pair.first;
                    node->rightChild = pair.second;

                    openList.push_back(node);
                }
                else {
                    auto node = BuildNodeFromSingleSphere(pair.first->collisionRepresentation);

                    node->leftChild = pair.first;

                    openList.push_back(node);
                }
            }

            maxDistanceBetweenLeaves = std::numeric_limits<float>::max();
        }

        return openList[0];
    }

    std::vector<entt::entity> BVH::FindPossibleCollisions(BVH::SphereNode* treeRoot, BVH::Sphere* sphere)
    {
        std::vector<entt::entity> possibleCollisions;

        if (!sphere || !treeRoot)
            return possibleCollisions;

        if (!SphereSphereTest(treeRoot->collisionRepresentation, sphere))
            return possibleCollisions;

        if (!treeRoot->leftChild && !treeRoot->rightChild)
        {
            possibleCollisions.push_back(treeRoot->entity);

            return possibleCollisions;
        }

        //std::cout << "Traversing Tree" << std::endl;
        
        auto collisions = FindPossibleCollisions(treeRoot->leftChild, sphere);

        possibleCollisions.insert(possibleCollisions.end(), collisions.begin(), collisions.end());

        collisions = FindPossibleCollisions(treeRoot->rightChild, sphere);

        possibleCollisions.insert(possibleCollisions.end(), collisions.begin(), collisions.end());

        return possibleCollisions;
    }
}