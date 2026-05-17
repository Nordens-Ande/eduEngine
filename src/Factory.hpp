#pragma once

#include "entt/entt.hpp"
#include "Components.hpp"
#include "ForwardRenderer.hpp"
#include "ShapeRenderer.hpp"
#include "RenderableMesh.hpp"
#include "InputManager.hpp"

//saving source here to not risk losing it, the link below shows where std::forward<Args> comes from
//https://gamedev.stackexchange.com/questions/176301/how-can-i-create-a-templated-function-to-add-components-to-their-necessary-conta
//https://en.cppreference.com/cpp/utility/forward

namespace ecs
{
	class Factory
	{
	public:
		template<typename T>
		static void AddComponent(entt::registry& registry, entt::entity entity, T component)
		{
			registry.emplace<T>(entity, component);
		};
		template<typename Component, typename... Args>
		static void AddComponent(entt::registry& registry, entt::entity entity, Args&&... args)
		{
			registry.emplace<Component>(entity, std::forward<Args>(args)...);
		}

		static entt::entity CreateEmpty(
			entt::registry& registry, 
			glm::vec3 position, 
			float rotation, 
			glm::vec3 scale
		);
		static entt::entity CreateHitbox(
			entt::registry& registry,
			glm::vec3 position,
			glm::vec3 size,
			bool isTrigger
		);
		static entt::entity CreateMesh(
			entt::registry& registry,
			eeng::ForwardRendererPtr renderer,
			std::shared_ptr<eeng::RenderableMesh> mesh,
			glm::vec3 position,
			float rotation,
			glm::vec3 scale
		);
		static entt::entity CreatePlayer(
			entt::registry& registry, 
			eeng::ForwardRendererPtr renderer, 
			InputManagerPtr input, 
			std::shared_ptr<eeng::RenderableMesh> mesh, 
			glm::vec3 position, 
			float rotation, 
			glm::vec3 scale, 
			float speed
		);
		static entt::entity CreateNPC(
			entt::registry& registry, 
			eeng::ForwardRendererPtr renderer, 
			std::shared_ptr<eeng::RenderableMesh> mesh, 
			glm::vec3 position, 
			float rotation, 
			glm::vec3 scale, 
			float speed
		);
		static entt::entity CreateControllerNPC(
			entt::registry& registry, 
			eeng::ForwardRendererPtr renderer, 
			std::shared_ptr<eeng::RenderableMesh> mesh, 
			glm::vec3 position, 
			float rotation, 
			glm::vec3 scale, 
			float speed, 
			std::vector<glm::vec3> points);
	};
}

