#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "spdlog/spdlog.h"

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/HealthComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

class DamageSystem: public System {
    public:
        DamageSystem() {
            RequireComponent<BoxColliderComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
        }

        void onCollision(CollisionEvent& event) {
            Entity a = event.a;
            Entity b = event.b;
            spdlog::info("Collision!" + std::to_string(a.GetId()) + " with " + std::to_string(b.GetId()));

            // Normalize
            if (b.InGroup("projectiles")) std::swap(a, b);
            if (!a.InGroup("projectiles")) return;

            Entity projectile = a;
            Entity target = b;

            if (target.HasTag("player")) {
                onPlayerHitByProjectile(projectile, target);
            }
            if (target.InGroup("enemies")) {
                onEnemyHitByProjectile(projectile, target);
            }

        }

        void onPlayerHitByProjectile(Entity projectile, Entity player) {
            applyProjectileDamage(projectile, player, false);
        }

        void onEnemyHitByProjectile(Entity projectile, Entity enemy) {
            applyProjectileDamage(projectile, enemy, true);
        }
        
        void applyProjectileDamage(Entity projectile, Entity target, bool isEnemy) {
            const auto& projectileComp = projectile.GetComponent<ProjectileComponent>();
            if (projectileComp.isFriendly != isEnemy) return;

            auto& health = target.GetComponent<HealthComponent>();
            health.healthPercentage -= projectileComp.hitPercentDamage;

            if (health.healthPercentage <= 0) {
                target.Kill();
            }
            projectile.Kill();
            spdlog::info("Entity " + std::to_string(target.GetId()) + " now has " + std::to_string(health.healthPercentage) + "% health.");
        }



        void Update(){

        }

};

#endif