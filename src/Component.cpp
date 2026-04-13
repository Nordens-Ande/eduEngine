#include<Component.hpp>
#include <entt/entt.hpp>
#include <glm/gtx/matrix_decompose.hpp>

void MovementSystem::Update(entt::registry& registry, float dt)
{
    for (auto entity : registry.view<TransformComponent, LinearVelocityComponent>())
    {
        auto& transform = registry.get<TransformComponent>(entity);
        auto& velocity = registry.get<LinearVelocityComponent>(entity);

        transform.transform = glm::translate(transform.transform, velocity.velocity * dt);
    }
}

void PlayerControllerSystem::Update(entt::registry& registry, float dt)
{
    for (entt::entity entity : registry.view<PlayerControllerComponent, LinearVelocityComponent>())
    {
        auto& controller = registry.get<PlayerControllerComponent>(entity);
        auto& vel = registry.get<LinearVelocityComponent>(entity);

        glm::vec3 moveDir { 0, 0, 0 };
        if (controller.inputManager->IsKeyPressed(eeng::InputManager::Key::W)) moveDir.z += 1.0f;
        if (controller.inputManager->IsKeyPressed(eeng::InputManager::Key::A)) moveDir.x += 1.0f;
        if (controller.inputManager->IsKeyPressed(eeng::InputManager::Key::S)) moveDir.z -= 1.0f;
        if (controller.inputManager->IsKeyPressed(eeng::InputManager::Key::D)) moveDir.x -= 1.0f;

        //std::cout << moveDir.x << std::endl;

        vel.velocity = moveDir * controller.speed;
    }
}

void RenderSystem::Render(entt::registry& registry) 
{
    for (entt::entity entity : registry.view<TransformComponent, MeshComponent>())
    {
        auto& transfrom = registry.get<TransformComponent>(entity);
        auto& mesh = registry.get<MeshComponent>(entity);

        //renderableMesh->
        //mesh.mesh->animate(middleCharacterAnimIndex, time * characterAnimSpeed);
        mesh.forwardRenderer->renderMesh(mesh.mesh, transfrom.transform);
        //character_aabb1 = renderableMesh->m_model_aabb.post_transform(characterWorldTransform.transform);
    }
}

void NPCControllerSystem::Update(entt::registry& registry, float dt)
{
    for (entt::entity entity : registry.view<NPCControllerComponent, TransformComponent, LinearVelocityComponent>())
    {
        auto& controller = registry.get<NPCControllerComponent>(entity);
        auto& transform = registry.get<TransformComponent>(entity);
        auto& velocity = registry.get<LinearVelocityComponent>(entity);
    
        //skips if there is no points present
        if (controller.points.size() == 0)
            continue;

        //decompose translate from entitys transform
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transform.transform, scale, rotation, translation, skew, perspective);

        glm::vec3 target = controller.points[controller.pointIndex];
        glm::vec3 relative = target - translation;

        if (glm::length(relative) < 0.1f) //close enough, go to next point
        {
            controller.pointIndex = (controller.pointIndex + 1) % controller.points.size();
        }
        else //else move to the current point
        {
            velocity.velocity = glm::normalize(relative) * controller.speed;
        }
    }
}