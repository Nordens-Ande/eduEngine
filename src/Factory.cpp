#include "Factory.hpp"

namespace ecs
{
	entt::entity Factory::CreateEmpty(entt::registry& registry, glm::vec3 position, float rotation, glm::vec3 scale)
	{
		entt::entity gameObject = registry.create();
		AddComponent<TransformComponent>(registry, gameObject, position, rotation, scale);

		return gameObject;
	}

	entt::entity Factory::CreateHitbox(entt::registry& registry, glm::vec3 position, glm::vec3 size, bool isTrigger = false)
	{
		entt::entity hitbox = CreateEmpty(registry, position, 0, glm::vec3{ 1, 1, 1 });

		eeng::AABB aabb = eeng::AABB{};
		aabb.min = position - (size * 0.5f);
		aabb.max = position + (size * 0.5f);

		AddComponent<AABBComponent>(registry, hitbox, false, aabb);
		AddComponent<SphereComponent>(registry, hitbox);
		AddComponent<ColliderComponent>(registry, hitbox, isTrigger);

		return hitbox;
	}

	entt::entity Factory::CreateMesh(entt::registry& registry, eeng::ForwardRendererPtr renderer, std::shared_ptr<eeng::RenderableMesh> mesh, glm::vec3 position, float rotation, glm::vec3 scale)
	{
		entt::entity meshObject = CreateEmpty(registry, position, rotation, scale);
		AddComponent<MeshComponent>(registry, meshObject, renderer, mesh);
		AddComponent<AABBComponent>(registry, meshObject);
		AddComponent<SphereComponent>(registry, meshObject);
		AddComponent<ColliderComponent>(registry, meshObject);

		return meshObject;
	}

	entt::entity Factory::CreatePlayer(entt::registry& registry, eeng::ForwardRendererPtr renderer, InputManagerPtr input, std::shared_ptr<eeng::RenderableMesh> mesh, glm::vec3 position, float rotation, glm::vec3 scale, float speed)
	{
		entt::entity player = CreateMesh(registry, renderer, mesh, position, rotation, scale);
		AddComponent<PlayerControllerComponent>(registry, player, speed, input);
		AddComponent<LinearVelocityComponent>(registry, player);
		
		eeng::AnimationBranchDesc upperBodyFilter;
		upperBodyFilter.root_node_name = "mixamorig:Spine";
		upperBodyFilter.mode = eeng::AnimationBranchDesc::Mode::IncludeSubtree;
		AddComponent<AnimationComponent>(registry, player, 1, 2, 0.5f, false, upperBodyFilter);

		return player;
	}

	entt::entity Factory::CreateNPC(entt::registry& registry, eeng::ForwardRendererPtr renderer, std::shared_ptr<eeng::RenderableMesh> mesh, glm::vec3 position, float rotation, glm::vec3 scale, float speed)
	{
		entt::entity npc = CreateMesh(registry, renderer, mesh, position, rotation, scale);
		AddComponent<LinearVelocityComponent>(registry, npc);

		return npc;
	}

	entt::entity Factory::CreateControllerNPC(entt::registry& registry, eeng::ForwardRendererPtr renderer, std::shared_ptr<eeng::RenderableMesh> mesh, glm::vec3 position, float rotation, glm::vec3 scale, float speed, std::vector<glm::vec3> points)
	{
		entt::entity npc = CreateNPC(registry, renderer, mesh, position, rotation, scale, speed);
		AddComponent<NPCControllerComponent>(registry, npc, speed, 0, points);

		return npc;
	}
}