
//#ifndef Component_hpp
//#define Component_hpp
#pragma once

#include <entt/entt.hpp>
#include "glmcommon.hpp"
#include "ForwardRenderer.hpp"
#include "ShapeRenderer.hpp"
#include "RenderableMesh.hpp"
#include "InputManager.hpp"
//#include "Game.cpp"

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
{ //fetched from Game.hpp camera struct:
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

class TPCameraSystem : public UpdateableSystem
{
public:
	void Update(entt::registry& registry, float dt) override;
};

class SkeletonGizmoSystem : public RenderableSystem
{
	void Render(entt::registry& registry) override;
};

class AnimationSystem : public UpdateableSystem
{
public:
	void Update(entt::registry& registry, float dt) override;
};
