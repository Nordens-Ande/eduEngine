#pragma once

#include "entt/entt.hpp"
#include "Components.hpp"

//Abstract classes (try to make something similar to C#s interfaces)
//class UpdateableSystem
//{
//public:
//	virtual void Update(entt::registry& registry, float dt) = 0;
//};
//class RenderableSystem
//{
//public:
//	virtual void Render(entt::registry& registry) = 0;
//};

namespace ecs
{

#pragma region Abstract Systems

	class UpdateableSystem
	{
	public:
		virtual void Update(entt::registry& registry, float dt) = 0;
		//virtual ~UpdateableSystem() = default;
	};
	class RenderableSystem
	{
	public:
		virtual void Render(entt::registry& registry) = 0;
		//virtual ~RenderableSystem() = default;
	};

#pragma endregion


#pragma region Templates

	template<typename Derived, typename... Components>
	class UpdateableSystemTemplate : public UpdateableSystem
	{
	public:
		void Update(entt::registry& registry, float dt) override
		{
			auto view = registry.view<Components...>(); //fetches the view for all defined components.

			for (entt::entity entity : view)
			{
				//converts the general (UpdateableSystem) object (this) to the defined derived type,
				//enabling the OnUpdate(...) call to be made.
				static_cast<Derived*>(this)->OnUpdate(registry, entity, registry.get<Components>(entity)..., dt);
			}
		}
	};

	template<typename Derived, typename... Components>
	class RenderableSystemTemplate : public RenderableSystem
	{
	public:
		void Render(entt::registry& registry) override
		{
			auto view = registry.view<Components...>(); //fetches the view for all defined components.

			for (entt::entity entity : view)
			{
				//converts the general (RenderableSystem) object (this) to the defined derived type,
				//enabling the OnRender(...) call to be made.
				static_cast<Derived*>(this)->OnRender(registry, entity, registry.get<Components>(entity)...);
			}
		}
	};

	//template<typename Derived, typename 

#pragma endregion


#pragma region Systems

	class SourceSystem /*: public UpdateableSystemTemplate<SourceSystem, SourceComponent>*/
	{
	public:
		//void OnUpdate(
		//	entt::registry& registry,
		//	entt::entity entity,
		//	SourceComponent&,
		//	float dt)
		//{

		//};
		void Notify(
			entt::registry& registry,
			entt::entity entity,
			SourceComponent& source,
			events::Events event
		);
		void AddObserver(
			SourceComponent& source,
			ObserverComponent* observer
		);
		void RemoveObserver(
			SourceComponent& source,
			ObserverComponent* observer
		);
	};

	class MovementSystem : public UpdateableSystemTemplate<MovementSystem, TransformComponent, LinearVelocityComponent>
	{
	public:
		void OnUpdate(
			entt::registry& registry,
			entt::entity entity, 
			TransformComponent& transform, 
			LinearVelocityComponent& vel, 
			float dt
		);
	};

	class PlayerControllerSystem : public UpdateableSystemTemplate<PlayerControllerSystem, PlayerControllerComponent, LinearVelocityComponent>
	{
	public:
		void OnUpdate(
			entt::registry& registry,
			entt::entity entity, 
			PlayerControllerComponent& controller, 
			LinearVelocityComponent& vel, 
			float dt
		);
	};

	class RenderSystem : public RenderableSystemTemplate<RenderSystem, TransformComponent, MeshComponent>
	{
	public:
		void OnRender(
			entt::registry& registry,
			entt::entity entity, 
			TransformComponent& transform, 
			MeshComponent& mesh
		);
	};

	class NPCControllerSystem : public UpdateableSystemTemplate<NPCControllerSystem, NPCControllerComponent, TransformComponent, LinearVelocityComponent>
	{
	public:
		void OnUpdate(
			entt::registry& registry,
			entt::entity entity, 
			NPCControllerComponent& controller, 
			TransformComponent& transform, 
			LinearVelocityComponent& vel, 
			float dt
		);
	};

	//class PointLightSystem : public UpdateableSystem
	//{
	//public:
	//	void Update(entt::registry& registry, float dt) override;
	//};

	class TPCameraSystem : public UpdateableSystemTemplate<TPCameraSystem, CameraComponent, TransformComponent>
	{
	public:
		void OnUpdate(
			entt::registry& registry,
			entt::entity entity,
			CameraComponent& camera,
			TransformComponent& transform,
			float dt
		);
	};

	class SkeletonGizmoSystem : public RenderableSystemTemplate<SkeletonGizmoSystem, TransformComponent, MeshComponent, GizmoComponent>
	{
	public:
		void OnRender(
			entt::registry& registry,
			entt::entity entity, 
			TransformComponent& transform, 
			MeshComponent& mesh, 
			GizmoComponent& gizmo
		);
	};

	class AnimationSystem : public UpdateableSystemTemplate<AnimationSystem, AnimationComponent, LinearVelocityComponent>
	{
	public:
		void OnUpdate(
			entt::registry& registry,
			entt::entity entity, 
			AnimationComponent& animation,
			LinearVelocityComponent& vel,
			float dt
		);
	};

	class AttackSystem : public SourceSystem, public UpdateableSystemTemplate<AttackSystem, TransformComponent, AttackComponent>
	{
	public:
		void OnUpdate(
			entt::registry& registry,
			entt::entity entity,
			TransformComponent& transform,
			AttackComponent& attack,
			float dt
		);
	};

	//class ObserverSystem : public UpdateableSystemTemplate<ObserverSystem, ObserverComponent>
	//{
	//public:
	//	void OnUpdate(
	//		entt::registry& registry,
	//		entt::entity entity,
	//		ObserverComponent&,
	//		float dt
	//	);
	//};

#pragma endregion

}

