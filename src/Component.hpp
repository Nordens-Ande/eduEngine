
//#ifndef Component_hpp
//#define Component_hpp
#pragma once

#include <entt/entt.hpp>
#include "glmcommon.hpp"
#include "RenderableMesh.hpp"
//#include 

//struct Position { glm::vec3 pos; };
//struct Position { glm::vec3 vel; };

struct TransformComponent
{
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
};

struct LinearVelocityComponent
{
	glm::vec3 velocity;
};

struct MeshComponent
{
	glm::vec3 velocity;
};

struct PlayerControllerComponent
{
	std::weak_ptr<eeng::RenderableMesh> mesh;
};

struct NPCController
{

};

struct PointLightComponent
{

};

struct CameraComponent
{

};



class ComponentSystem
{
public:
	virtual void Update(entt::registry& registry, float dt) = 0;
};

class MovementSystem : ComponentSystem
{
	void Update(entt::registry& registry, float dt) override;
};

class PlayerController : ComponentSystem
{
	void Update(entt::registry& registry, float dt) override;
};

class RenderSystem : ComponentSystem
{
	void Update(entt::registry& registry, float dt) override;
};

class NPCControllerSystem : ComponentSystem
{
	void Update(entt::registry& registry, float dt) override;
};

class PointLightSystem : ComponentSystem
{
	void Update(entt::registry& registry, float dt) override;
};

class CameraSystem : ComponentSystem
{
	void Update(entt::registry& registry, float dt) override;
};
