
//#ifndef Component_hpp
//#define Component_hpp
#pragma once

#include <entt/entt.hpp>
#include "glmcommon.hpp"
#include "ForwardRenderer.hpp"
#include "RenderableMesh.hpp"
#include "InputManager.hpp"

//struct Position { glm::vec3 pos; };
//struct Position { glm::vec3 vel; };

struct TransformComponent
{
	//glm::vec3 position;
	//glm::vec3 scale;
	//glm::vec3 rotation;
	glm::mat4 transform;
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
{
	//TODO
};


//Abstract classes (try to make something similar to C#s interfaces)
class UpdateableSystem
{
public:
	virtual void Update(entt::registry& registry, float dt) = 0;
};
class RenderableSystem
{
public:
	virtual void Render(entt::registry& registry) = 0;
};

//Systems:
class MovementSystem : public UpdateableSystem
{
public:
	void Update(entt::registry& registry, float dt) override;
};

class PlayerControllerSystem : public UpdateableSystem
{
public:
	void Update(entt::registry& registry, float dt) override;
};

class RenderSystem : public RenderableSystem
{
public:
	void Render(entt::registry& registry) override;
};

class NPCControllerSystem : public UpdateableSystem
{
public:
	void Update(entt::registry& registry, float dt) override;
};

class PointLightSystem : public UpdateableSystem
{
public:
	void Update(entt::registry& registry, float dt) override;
};

class CameraSystem : public UpdateableSystem
{
public:
	void Update(entt::registry& registry, float dt) override;
};
