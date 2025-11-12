#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "spdlog/spdlog.h"

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
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
            spdlog::info("Collision!" + std::to_string(event.a.GetId()));
        }

        void Update(){

        }


};

#endif