#ifndef PROJECTILEEMITSYSTEM_H
#define PROJECTILEEMITSYSTEM_H

#include <SDL2/SDL.h>
#include "spdlog/spdlog.h"

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/FireControlComponent.h"
#include "../Components/DirectionComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

class ProjectileEmitSystem: public System {
    private:
        Registry* registry;

    public:
        ProjectileEmitSystem() {
            RequireComponent<ProjectileEmitterComponent>();
            RequireComponent<TransformComponent>();
            registry = nullptr;
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::onPress);
        }

        void onPress(KeyPressedEvent& event) {
            spdlog::info("Fire Key Pressed!");
            if (!registry) return;

            for (auto entity: GetSystemEntities()) {
                if (entity.HasComponent<FireControlComponent>() && event.key == SDLK_SPACE) {
                    auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                    _emitProjectile(&entity, projectileEmitter);
                }
            }
        }

        void Update(std::unique_ptr<Registry>& registry) {
            this->registry = registry.get();

            for (auto entity: GetSystemEntities()) {
                auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

                if (SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency
                    && !entity.HasComponent<FireControlComponent>()) {
                    _emitProjectile(&entity, projectileEmitter);
                    projectileEmitter.lastEmissionTime = SDL_GetTicks();
                }
            }
        }

        void _emitProjectile(Entity* entity, ProjectileEmitterComponent& projectileEmitter) {
            const auto& transform = entity->GetComponent<TransformComponent>();
            glm::vec2 projectilePosition = transform.position;
            if (entity->HasComponent<SpriteComponent>()) {
                const auto& sprite = entity->GetComponent<SpriteComponent>();
                projectilePosition.x += (transform.scale.x * sprite.width / 2);
                projectilePosition.y += (transform.scale.y * sprite.height / 2);
            }
            glm::vec2 projectileVelocity = projectileEmitter.projectileVelocity;
            if (entity->HasComponent<DirectionComponent>()) {
                const auto& direction = entity->GetComponent<DirectionComponent>();
                float speed = glm::length(projectileEmitter.projectileVelocity);
                projectileVelocity = direction.direction * speed;
            }
            Entity projectile = registry->CreateEntity();
            projectile.AddGroup("projectiles");
            projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDuration);
            projectile.AddComponent<TransformComponent>(projectilePosition, glm::vec2(1.0, 1.0), 0.0);
            projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
            projectile.AddComponent<BoxColliderComponent>(4, 4);
            projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
        }

};

#endif