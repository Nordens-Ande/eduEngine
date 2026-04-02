#include<Component.hpp>
#include <entt/entt.hpp>

void MovementSystem::Update(entt::registry& registry, float dt)
{
    for (auto entity : registry.view<TransformComponent, LinearVelocityComponent>())
    {
        auto& transform = registry.get<TransformComponent>(entity);
        auto& velocity = registry.get<LinearVelocityComponent>(entity);

        transform.position += velocity.velocity * dt;
    }
}