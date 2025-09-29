#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

class MovementSystem: public System {

    public:
        MovementSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();
        }

        void Update() {
            // TODO: Update entity position based on its velocity
            for (auto entity: GetSystemEntities()) {
                TransformComponent& transform = entity.GetComponent<TransformComponent>();
                const RigidBodyComponent rigidbody = entity.GetComponent<RigidBodyComponent>();

                transform.position.x += rigidbody.velocity.x;
                transform.position.y += rigidbody.velocity.y;

            }
        }
};

#endif