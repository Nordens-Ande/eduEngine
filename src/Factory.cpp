#include "Factory.hpp"

namespace ecs
{
	entt::entity Factory::CreateEmpty(entt::registry& registry, glm::vec3 position, float rotation, glm::vec3 scale)
	{
		entt::entity gameObject = registry.create();
		AddComponent<TransformComponent>(registry, gameObject, position, rotation, scale);

		return gameObject;
	}

	entt::entity Factory::CreatePlayer(entt::registry& registry, eeng::ForwardRendererPtr renderer, InputManagerPtr input, std::shared_ptr<eeng::RenderableMesh> mesh, glm::vec3 position, float rotation, glm::vec3 scale, float speed)
	{
		entt::entity player = CreateEmpty(registry, position, rotation, scale);
		AddComponent<MeshComponent>(registry, player, renderer, mesh);
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
		entt::entity npc = CreateEmpty(registry, position, rotation, scale);
		AddComponent<MeshComponent>(registry, npc, renderer, mesh);
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