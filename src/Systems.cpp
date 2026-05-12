#include "Systems.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include "glmcommon.hpp"

namespace ecs
{
    void MovementSystem::OnUpdate(
        entt::registry& registry,
        entt::entity entity, 
        TransformComponent& transform, 
        LinearVelocityComponent& vel, 
        float dt)
    {
        transform.position += vel.velocity * dt;
    }

    void PlayerControllerSystem::OnUpdate(
        entt::registry& registry,
        entt::entity entity,
        PlayerControllerComponent& controller,
        LinearVelocityComponent& vel,
        float dt)
    {
        glm::vec3 moveDir{ 0, 0, 0 };
        if (controller.inputManager->IsKeyPressed(eeng::InputManager::Key::W)) moveDir.z += 1.0f;
        if (controller.inputManager->IsKeyPressed(eeng::InputManager::Key::A)) moveDir.x += 1.0f;
        if (controller.inputManager->IsKeyPressed(eeng::InputManager::Key::S)) moveDir.z -= 1.0f;
        if (controller.inputManager->IsKeyPressed(eeng::InputManager::Key::D)) moveDir.x -= 1.0f;

        vel.velocity = moveDir * controller.speed;
    }

    void RenderSystem::OnRender(
        entt::registry& registry,
        entt::entity entity,
        TransformComponent& transform,
        MeshComponent& mesh)
    {
        if (auto* animation = registry.try_get<AnimationComponent>(entity))
        {
            if (animation->useLayering)
                mesh.mesh->animateBlend(animation->primaryAnimation, animation->secondaryAnimation, animation->time, animation->time, animation->filter);
            else
                mesh.mesh->animateBlend(animation->primaryAnimation, animation->secondaryAnimation, animation->time, animation->time, animation->blendFactor);
        }

        mesh.forwardRenderer->renderMesh(mesh.mesh, transform.getTransform());
    }

    void NPCControllerSystem::OnUpdate(
        entt::registry& registry,
        entt::entity entity,
        NPCControllerComponent& controller,
        TransformComponent& transform,
        LinearVelocityComponent& vel,
        float dt)
    {
        //skips if there is no points present
        if (controller.points.size() == 0)
            return;

        //decompose translate from entitys transform
        //glm::vec3 scale;
        //glm::quat rotation;
        //glm::vec3 translation;
        //glm::vec3 skew;
        //glm::vec4 perspective;
        //glm::decompose(transform.transform, scale, rotation, translation, skew, perspective);

        glm::vec3 target = controller.points[controller.pointIndex];
        glm::vec3 relative = target - transform.position;

        if (glm::length(relative) < 0.1f) //close enough, go to next point
        {
            controller.pointIndex = (controller.pointIndex + 1) % controller.points.size();
        }
        else //else move to the current point
        {
            vel.velocity = glm::normalize(relative) * controller.speed;
        }
    }

    void TPCameraSystem::OnUpdate(
        entt::registry& registry,
        entt::entity entity,
        CameraComponent& camera,
        TransformComponent& transform,
        float dt)
    {
        InputManagerPtr input = camera.inputManager;

        camera.lookAt = transform.position;

        auto mouse = input->GetMouseState();
        glm::ivec2 mouse_xy{ mouse.x, mouse.y };
        glm::ivec2 mouse_xy_diff{ 0, 0 };
        if (mouse.leftButton && camera.mouse_xy_prev.x >= 0)
            mouse_xy_diff = camera.mouse_xy_prev - mouse_xy;
        camera.mouse_xy_prev = mouse_xy;

        // Update camera rotation from mouse movement
        camera.yaw += mouse_xy_diff.x * camera.sensitivity;
        camera.pitch += mouse_xy_diff.y * camera.sensitivity;
        camera.pitch = glm::clamp(camera.pitch, -glm::radians(89.0f), 0.0f);

        // Update camera position
        const glm::vec4 rotatedPos = glm_aux::R(camera.yaw, camera.pitch) * glm::vec4(0.0f, 0.0f, camera.distance, 1.0f);
        camera.pos = camera.lookAt + glm::vec3(rotatedPos);
    }

    void SkeletonGizmoSystem::OnRender(
        entt::registry& registry,
        entt::entity entity,
        TransformComponent& transform,
        MeshComponent& mesh,
        GizmoComponent& gizmo)
    {
        if (!gizmo.isEnabled) 
            return;

        std::shared_ptr<eeng::RenderableMesh> characterMesh = mesh.mesh;
        ShapeRendererPtr shapeRenderer = gizmo.shapeRenderer;
        float axisLen = 25.0f;

        for (int i = 0; i < characterMesh->boneMatrices.size(); ++i) {
            auto IBinverse = glm::inverse(characterMesh->m_bones[i].inversebind_tfm);
            glm::mat4 global = transform.getTransform() * characterMesh->boneMatrices[i] * IBinverse;
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
    };

    void AnimationSystem::OnUpdate(
        entt::registry& registry,
        entt::entity entity,
        AnimationComponent& animation,
        LinearVelocityComponent& vel,
        float dt)
    {
        animation.time += dt;
        if (!animation.useLayering)
        {
            float blend = std::lerp(animation.blendFactor, glm::length(vel.velocity) > 0.01f ? 1.0f : 0.0f, 0.5f);
            //animation.blendFactor = glm::length(velocity.velocity) > 0.01f ? 1 : 0;
            animation.blendFactor = blend;
        }
    }
}