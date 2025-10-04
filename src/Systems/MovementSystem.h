#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "spdlog/spdlog.h"

class MovementSystem: public System {

    public:
        MovementSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();
        }

        void Update(double deltaTime) {
            // TODO: Move to cpp 
            for (auto entity: GetSystemEntities()) {
                TransformComponent& transform = entity.GetComponent<TransformComponent>();
                const RigidBodyComponent rigidbody = entity.GetComponent<RigidBodyComponent>();

                transform.position.x += rigidbody.velocity.x * deltaTime;
                transform.position.y += rigidbody.velocity.y * deltaTime;

            }
        }
};

#endif