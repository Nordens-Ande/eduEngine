#include<Component.hpp>
#include <entt/entt.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "glmcommon.hpp"

void MovementSystem::Update(entt::registry& registry, float dt)
{
    for (entt::entity entity : registry.view<TransformComponent, LinearVelocityComponent>())
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
        auto& transform = registry.get<TransformComponent>(entity);
        auto& mesh = registry.get<MeshComponent>(entity);

        //renderableMesh->
        if (auto* animation = registry.try_get<AnimationComponent>(entity))
        {
            if (animation->useLayering)
                mesh.mesh->animateBlend(animation->primaryAnimation, animation->secondaryAnimation, animation->time, animation->time, animation->filter);
            else
                mesh.mesh->animateBlend(animation->primaryAnimation, animation->secondaryAnimation, animation->time, animation->time, animation->blendFactor);
        }
        mesh.forwardRenderer->renderMesh(mesh.mesh, transform.transform);
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

void TPCameraSystem::Update(entt::registry& registry, float dt)
{
    for (entt::entity entity : registry.view<TransformComponent, CameraComponent>())
    {
        auto& transform = registry.get<TransformComponent>(entity);
        auto& camera = registry.get<CameraComponent>(entity);
    }
}

void SkeletonGizmoSystem::Render(entt::registry& registry)
{
    for (entt::entity entity : registry.view<TransformComponent, MeshComponent, GizmoComponent>())
    {
        auto& transform = registry.get<TransformComponent>(entity);
        auto& mesh = registry.get<MeshComponent>(entity);
        auto& gizmo = registry.get<GizmoComponent>(entity);

        if (!gizmo.isEnabled) continue;

        std::shared_ptr<eeng::RenderableMesh> characterMesh = mesh.mesh;
        ShapeRendererPtr shapeRenderer = gizmo.shapeRenderer;
        float axisLen = 25.0f;

        for (int i = 0; i < characterMesh->boneMatrices.size(); ++i) {
            auto IBinverse = glm::inverse(characterMesh->m_bones[i].inversebind_tfm);
            glm::mat4 global = transform.transform * characterMesh->boneMatrices[i] * IBinverse;
            glm::vec3 pos = glm::vec3(global[3]);

            glm::vec3 right = glm::vec3(global[0]); // X
            glm::vec3 up = glm::vec3(global[1]); // Y
            glm::vec3 fwd = glm::vec3(global[2]); // Z

            shapeRenderer->push_states(ShapeRendering::Color4u::Red);
            shapeRenderer->push_line(pos, pos + axisLen * right);

            shapeRenderer->push_states(ShapeRendering::Color4u::Green);
            shapeRenderer->push_line(pos, pos + axisLen * up);

            shapeRenderer->push_states(ShapeRendering::Color4u::Blue);
            shapeRenderer->push_line(pos, pos + axisLen * fwd);

            shapeRenderer->pop_states<ShapeRendering::Color4u>();
            shapeRenderer->pop_states<ShapeRendering::Color4u>();
            shapeRenderer->pop_states<ShapeRendering::Color4u>();
        }
    }
};

void AnimationSystem::Update(entt::registry& registry, float dt)
{
    for (entt::entity entity : registry.view<AnimationComponent, LinearVelocityComponent>())
    {
        auto& animation = registry.get<AnimationComponent>(entity);
        auto& velocity = registry.get<LinearVelocityComponent>(entity);

        animation.time += dt;
        if (!animation.useLayering)
        {
            float blend = std::lerp(animation.blendFactor, glm::length(velocity.velocity) > 0.01f ? 1.0f : 0.0f, 0.5f);
            //animation.blendFactor = glm::length(velocity.velocity) > 0.01f ? 1 : 0;
            animation.blendFactor = blend;
        }
    }
}