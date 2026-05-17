#include "Systems.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include "glmcommon.hpp"
#include "imgui.h"

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
        
        controller.isInteracting = controller.inputManager->IsKeyPressed(eeng::InputManager::Key::E);
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
    };

    void AttackSystem::OnUpdate(
        entt::registry& registry,
        entt::entity entity,
        TransformComponent& transform,
        AttackComponent& attack,
        float dt)
    {
        if (!attack.isAttacking)
            return;

        attack.isAttacking = false;

        float closestDistance = -1;
        entt::entity closestNPC;

        for (entt::entity npc : registry.view<TransformComponent, NPCControllerComponent>())
        {
            TransformComponent& npcTrans = registry.get<TransformComponent>(npc);
            float distance = glm::distance(transform.position, npcTrans.position);
            if (distance < closestDistance || closestDistance == -1)
            {
                closestDistance = distance;
                closestNPC = npc;
            }
        }

        if (closestDistance != -1)
        {
            std::cout << "Found suitable NPC to attack, proccesing events..." << std::endl;

            //if (auto* source = registry.try_get<SourceComponent>(entity))
            //    this->Notify(registry, entity, *source, events::EVENT_ATTACKED);
            //if (auto* source = registry.try_get<SourceComponent>(closestNPC))
            //    this->Notify(registry, closestNPC, *source, events::EVENT_DIED);
            TryNotify(registry, entity, closestNPC, events::EVENT_ATTACKED);
            TryNotify(registry, closestNPC, entity, events::EVENT_DIED);
        }
    }

    void SourceSystem::Notify(
        entt::registry& registry,
        entt::entity sourceEntity,
        entt::entity selfEntity,
        ecs::SourceComponent& source,
        events::Events event)
    {
        for (int i = 0; i < source.numberOfObservers; i++)
        {
            //std::cout << "Notifying" << source.numberOfObservers << std::endl;
            source.observers[i]->OnNotify(registry, sourceEntity, selfEntity, event);
        }
    };
    bool SourceSystem::TryNotify(
        entt::registry& registry,
        entt::entity sourceEntity,
        entt::entity selfEntity,
        events::Events event)
    {
        if (auto* source = registry.try_get<ecs::SourceComponent>(sourceEntity))
        {
            this->Notify(registry, sourceEntity, selfEntity, *source, event);
            return true;
        }
        return false;
    };
    void SourceSystem::AddObserver(
        ecs::SourceComponent& source,
        ecs::ObserverComponent* observer)
    {
        source.observers[source.numberOfObservers] = observer;
        source.numberOfObservers++;
    }
    void SourceSystem::RemoveObserver(
        ecs::SourceComponent& source,
        ecs::ObserverComponent* observer)
    {
        int index = -1;
        for (int i = 0; i < source.numberOfObservers; ++i)
        {
            if (source.observers[i] != observer)
                continue;
            index = i;
            break;
        }

        if (index == -1)
            return;

        for (int i = index; i < source.numberOfObservers - 1; ++i)
        {
            source.observers[i] = source.observers[i + 1];
        }
        source.numberOfObservers--;
    }

    void AABBColliderSystem::OnRender(
        entt::registry& registry,
        entt::entity entity,
        TransformComponent& transform,
        MeshComponent& mesh,
        AABBComponent& aabb)
    {
        if (aabb.reSizeToMesh)
            aabb.collider = mesh.mesh->m_model_aabb.post_transform(transform.getTransform());
    }

    void AABBGizmoSystem::OnRender(
        entt::registry& registry,
        entt::entity entity,
        TransformComponent& transform,
        AABBComponent& aabb,
        GizmoComponent& gizmo)
    {
        if (gizmo.isEnabled)
            gizmo.shapeRenderer->push_AABB(aabb.collider.min, aabb.collider.max);
    }

    void SphereColliderSystem::OnRender(
        entt::registry& registry,
        entt::entity entity,
        TransformComponent& transform,
        SphereComponent& sphere)
    {
        if (!sphere.useAABB)
        {
            sphere.position = transform.position;
        }
        else if (auto* aabb = registry.try_get<AABBComponent>(entity))
        {
            glm::vec4 sphereFromAABB = aabb->collider.getBoundingSphere();
            sphere.position = sphereFromAABB;
            if (sphere.useMaxRadius)
            {
                if (sphere.radius < sphereFromAABB.w)
                    sphere.radius = sphereFromAABB.w;
            }
            else
            {
                sphere.radius = sphereFromAABB.w;
            }
        }
    }

    void SphereGizmoSystem::OnRender(
        entt::registry& registry,
        entt::entity entity,
        TransformComponent& transform,
        SphereComponent& sphere,
        GizmoComponent& gizmo)
    {
        if (gizmo.isEnabled)
        {
            gizmo.shapeRenderer->push_states(glm_aux::TS(sphere.position, glm::vec3(1.0f, 1.0f, 1.0f)));
            gizmo.shapeRenderer->push_sphere_wireframe(sphere.radius, sphere.radius);
            gizmo.shapeRenderer->pop_states<glm::mat4>();
        }
    }

    void WorldGUISystem::OnUpdate(
        entt::registry& registry,
        entt::entity entity,
        TransformComponent& transform,
        WorldGUIComponent& gui,
        float dt)
    {
        for (auto& pair : gui.elements)
        {
            pair.second -= dt;
        }
        gui.elements.erase(
            std::remove_if(gui.elements.begin(), gui.elements.end(),
                [](const auto& pair)
                {
                    return pair.second < 0.0f;
                }),
            gui.elements.end()
        );
    }
    void WorldGUISystem::OnRender(
        entt::registry& registry,
        entt::entity entity,
        TransformComponent& transform,
        WorldGUIComponent& gui)
    {
        for (int i = 0; i < gui.elements.size(); i++)
        {
            auto& pair = gui.elements[i];

            auto world_pos = transform.position + glm::vec3(0, gui.startHeight, 0);
            glm::ivec2 window_coords;
            if (glm_aux::window_coords_from_world_pos(world_pos, VP_P_V, window_coords))
            {
                // Draw an ImGui label at the projected window coordinates
                ImGui::SetNextWindowPos(
                    ImVec2{ float(window_coords.x), float(windowSize.y - window_coords.y - 25 * i) },
                    ImGuiCond_Always,
                    ImVec2{ 0.0f, 0.0f });
                ImGui::PushStyleColor(ImGuiCol_WindowBg, 0x80000000);
                ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);

                ImGuiWindowFlags flags =
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_NoInputs |
                    // ImGuiWindowFlags_NoBackground |
                    ImGuiWindowFlags_AlwaysAutoResize;

                std::string name = "text_box" + std::to_string(i);
                if (ImGui::Begin(name.c_str(), nullptr, flags))
                {
                    ImGui::Text("%s", pair.first.c_str());
                }
                ImGui::End();
                ImGui::PopStyleColor(2);
            }
        }
    }

    bool CollisionSystem::SphereSphereTest(SphereComponent a, SphereComponent b, glm::vec3& normal, float& penetration)
    {
        glm::vec3 centerToCenter = a.position - b.position;
        float distance = glm::dot(centerToCenter, centerToCenter);

        float radiusSum = a.radius + b.radius;
        
        penetration = radiusSum - glm::length(centerToCenter);
        normal = glm::normalize(centerToCenter);

        return distance <= radiusSum*radiusSum;
    }
    bool CollisionSystem::AABBAABBTest(AABBComponent a, AABBComponent b, glm::vec3& normal, float& penetration)
    {
        glm::vec3 aMax = a.collider.max;
        glm::vec3 aMin = a.collider.min;
        glm::vec3 bMax = b.collider.max;
        glm::vec3 bMin = b.collider.min;

        if (aMax.x < bMin.x || aMin.x > bMax.x) return false;
        if (aMax.y < bMin.y || aMin.y > bMax.y) return false;
        if (aMax.z < bMin.z || aMin.z > bMax.z) return false;

        float overlapX = std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x);
        float overlapY = std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y);
        float overlapZ = std::min(aMax.z, bMax.z) - std::max(aMin.z, bMin.z);

        glm::vec3 aCenter = (aMin + aMax) * 0.5f;
        glm::vec3 bCenter = (bMin + bMax) * 0.5f;

        penetration = overlapX;
        normal = {aCenter.x - bCenter.x, 0.0f, 0.0f };

        if (overlapY < penetration)
        {
            penetration = overlapY;
            normal = {0.0f, aCenter.y - bCenter.y, 0.0f};
        }

        if (overlapZ < penetration)
        {
            penetration = overlapZ;
            normal = {0.0f, 0.0f, aCenter.z - bCenter.z};
        }

        return true;
    }

    void CollisionSystem::BuildBVH(
        entt::registry& registry,
        float maxDistanceBetweenLeaves)
    {
        auto colliders = registry.view<ColliderComponent, SphereComponent>();
        
        std::vector<entt::entity> collidableEntities;
        for (entt::entity entity : colliders)
            collidableEntities.push_back(entity);
        
        root = bvh.BuildBVHBottomUp(registry, collidableEntities, maxDistanceBetweenLeaves);
    }
    void CollisionSystem::BuildEventQueue(
        entt::registry& registry)
    {
        eventQueue = events::EventQueue();
    }
    void CollisionSystem::Update(
        entt::registry& registry,
        float dt)
    {
        //std::cout << "Trying to Update CollisionSystem" << std::endl;
        BuildBVH(registry, 1000);

        //std::cout << root->collisionRepresentation->radius << std::endl;

        using Base = UpdateableSystemTemplate<CollisionSystem, ColliderComponent, SphereComponent>;
        Base::Update(registry, dt);

        //if (eventQueue.numberOfEventsInQueue > 0)
        //    std::cout << "EventQueue has: " << std::to_string(eventQueue.numberOfEventsInQueue) << " events" << std::endl;
        eventQueue.BroadcastAllEvents();
    }
    void CollisionSystem::OnUpdate(
        entt::registry& registry,
        entt::entity entity,
        ColliderComponent& collider,
        SphereComponent& sphere,
        float dt)
    {
        //std::cout << "OnUpdate for collision?" << std::endl;

        //Broad Phase
        std::vector<entt::entity> collisions = bvh.FindPossibleCollisions(root, &sphere);
        //if (collisions.size() > 1)
        //    std::cout << "Current possible collisions: " << collisions.size() << std::endl;

        //Narrow Phase (only done in 2 steps or more if the entity has more than one colliders, i.e also has an AABB)
        for (entt::entity other : collisions)
        {
            if (entity == other)
                continue;

            SphereComponent& sphereOther = bvh.registry->get<SphereComponent>(other);
            ColliderComponent* colliderOther = registry.try_get<ColliderComponent>(other);

            glm::vec3 normal;
            float penetration;

            //std::cout << "First iteration of narrow phase" << std::endl;
            //First step, where we check:
            // - Both are colliders
            // - Other collider is a trigger
            // - Both are not trigger colliders (if both are, we ignore)
            // - Do a SphereSphereTest (first part of the narrow-phase)
            if (!colliderOther || colliderOther->isTrigger || (collider.isTrigger && colliderOther->isTrigger) || !SphereSphereTest(sphere, sphereOther, normal, penetration))
                continue;


            AABBComponent* aabb = registry.try_get<AABBComponent>(entity);
            AABBComponent* aabbOther = bvh.registry->try_get<AABBComponent>(other);

            //Second step (AABB collision check, if there are any)
            if (aabb && aabbOther)
            {
                if (AABBAABBTest(*aabb, *aabbOther, normal, penetration))
                {
                    //if (collider.isTrigger)
                    //    std::cout << "Proccesing trigger event" << std::endl;

                    if (auto gui = registry.try_get<WorldGUIComponent>(entity))
                        if (!collider.isTrigger)
                            gui->elements.push_back(std::make_pair("*thump*", 1.0f));
                    eventQueue.EnqueueEvent(events::CollisionEvent{ &registry, entity, other, normal, penetration });
                }
            }
            else
            {
                if (auto gui = registry.try_get<WorldGUIComponent>(entity))
                    if (!collider.isTrigger)
                        gui->elements.push_back(std::make_pair("*circular thump*", 1.0f));
                eventQueue.EnqueueEvent(events::CollisionEvent{ &registry, entity, other, normal, penetration });
            }
        }
    }

    void TriggerSystem::OnUpdate(
        entt::registry& registry,
        entt::entity entity,
        ColliderComponent& collider,
        TriggerComponent& trigger,
        float dt)
    {
        //if (!trigger.isActive)
        //    return;

        //if (collider.isTrigger)
        //    std::cout << "Setting trigger to not active" << std::endl;
        //
        //trigger.isActive = false;
    }

    void FeedSystem::OnUpdate(
        entt::registry& registry,
        entt::entity entity,
        PlayerControllerComponent& controller,
        AnimationComponent& animation,
        FoodComponent& food,
        float dt)
    {
        if (controller.isInteracting)
        {
            if (food.feedingTime == 0)
            {
                animation.time = 0;
                animation.primaryAnimation = 3;
                animation.secondaryAnimation = 3;
            }
            food.feedingTime += dt;
        }
        else
        {
            food.feedingTime = 0;
            animation.primaryAnimation = 1;
            animation.secondaryAnimation = 2;
        }

        //if (food.feedingTime > food.durationForFeeding)

    }

}