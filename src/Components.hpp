
//#ifndef Component_hpp
//#define Component_hpp
#pragma once

#include <entt/entt.hpp>
#include <functional>
#include "glmcommon.hpp"
#include "ForwardRenderer.hpp"
#include "ShapeRenderer.hpp"
#include "RenderableMesh.hpp"
#include "InputManager.hpp"
#include "Events.hpp"
#include "AABB.h"
#include "BVH.hpp"

//struct Position { glm::vec3 pos; };
//struct Position { glm::vec3 vel; };

namespace ecs
{
	struct TransformComponent
	{
		glm::vec3 position;
		float rotation;
		glm::vec3 scale;
		//glm::mat4 transform;
		glm::mat4 getTransform()
		{
			return glm_aux::TRS(position, rotation, glm::vec3(0, 1, 0), scale);
		};
	};

	struct LinearVelocityComponent
	{
		//float maxSpeed;
		glm::vec3 velocity;
	};

	struct MeshComponent
	{
		eeng::ForwardRendererPtr forwardRenderer;
		std::shared_ptr<eeng::RenderableMesh> mesh;
	};

	struct PlayerControllerComponent
	{
		float speed;
		InputManagerPtr inputManager;
		bool isInteracting = false;
	};

	struct NPCControllerComponent
	{
		float speed;
		int pointIndex = 0;
		std::vector<glm::vec3> points;
	};

	struct PointLightComponent // Optional
	{
		//TODO
	};

	struct CameraComponent // Optional
	{ //fetched from Game.hpp camera struct:
		InputManagerPtr inputManager;

		glm::vec3 lookAt = glm_aux::vec3_000;   // Point of interest
		glm::vec3 up = glm_aux::vec3_010;       // Local up-vector
		float distance = 15.0f;                 // Distance to point-of-interest
		float sensitivity = 0.005f;             // Mouse sensitivity
		const float nearPlane = 1.0f;           // Rendering near plane
		const float farPlane = 500.0f;          // Rendering far plane

		// Position and view angles (computed when camera is updated)
		float yaw = 0.0f;                       // Horizontal angle (radians)
		float pitch = -glm::pi<float>() / 8;    // Vertical angle (radians)
		glm::vec3 pos;                          // Camera position

		// Previous mouse position
		glm::ivec2 mouse_xy_prev{ -1, -1 };
	};

	struct GizmoComponent
	{
		ShapeRendererPtr shapeRenderer;
		std::shared_ptr<eeng::RenderableMesh> mesh;
		bool isEnabled = true;
	};

	struct AnimationComponent
	{
		int primaryAnimation;
		int secondaryAnimation;
		float blendFactor;
		bool useLayering;
		eeng::AnimationBranchDesc filter;
		float time = 0;
	};

	struct AttackComponent
	{
		int damage = 1;
		bool isAttacking = false;
	};

	//https://stackoverflow.com/questions/35875616/c-store-functor-in-class
	struct ObserverComponent
	{
		std::function<void(entt::registry& registry, entt::entity source, entt::entity self, events::Events event)> OnNotify;
	};

	struct SourceComponent
	{
		ObserverComponent* observers[256];
		int numberOfObservers = 0;
	};

	struct ColliderComponent
	{
		bool isTrigger = false;
	};

	struct AABBComponent
	{
		bool reSizeToMesh = true;
		eeng::AABB collider;
	};

	struct SphereComponent
	{
		bool useAABB = true;
		bool useMaxRadius = true;
		glm::vec3 position;
		float radius;
	};

	struct WorldGUIComponent
	{
		float startHeight = 3.5f;
		std::vector<std::pair<std::string, float>> elements;
	};

	struct FoodComponent
	{
		bool hasFood;
		float durationForFeeding = 5;
		float feedingTime = 0;
	};

	struct TriggerComponent
	{
		std::function<void(entt::registry* registry, entt::entity triggerEntity, entt::entity enteredEntity)> OnTrigger;
		//bool isActive = false;
	};
}