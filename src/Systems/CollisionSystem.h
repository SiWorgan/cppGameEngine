#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "spdlog/spdlog.h"

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"

class CollisionSystem: public System {
    public:
        CollisionSystem() {
            RequireComponent<BoxColliderComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update() {
            auto entities = GetSystemEntities();
            for (auto entity : entities) {
                auto& collider = entity.GetComponent<BoxColliderComponent>();
                collider.colliding = false;
            }

            for (auto i = entities.begin(); i != entities.end(); i++) {
                Entity a = *i;
                auto aTransform = a.GetComponent<TransformComponent>();
                auto& aCollider = a.GetComponent<BoxColliderComponent>();
                for (auto j = i+1; j != entities.end(); j++) {
                    Entity b = *j;

                    auto bTransform = b.GetComponent<TransformComponent>();
                    auto& bCollider = b.GetComponent<BoxColliderComponent>();
                    bool collision = CheckAABBCollision(aTransform, aCollider, bTransform, bCollider);

                    if (collision) {
                        aCollider.colliding = true;
                        bCollider.colliding = true;
                        spdlog::info("Collision Happened!");
                    };
                }
            } 
        }
    
    private:
        bool CheckAABBCollision(TransformComponent At, BoxColliderComponent Ab , 
                                    TransformComponent Bt, BoxColliderComponent Bb) {
            auto aPx = At.position.x + Ab.offset.x;
            auto aPy = At.position.y + Ab.offset.y;
            auto bPx = Bt.position.x + Bb.offset.x;
            auto bPy = Bt.position.y + Bb.offset.y;
            return (
                aPx < bPx + Bb.width &&
                aPx + Ab.width > bPx &&
                aPy < bPy + Bb.height &&
                aPy + Ab.height > bPy 
            );
        }
};

#endif