#ifndef RENDERCOLLISIONSYSTEM_H
#define RENDERCOLLISIONSYSTEM_H

#include <SDL2/SDL.h>
#include "spdlog/spdlog.h"

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"


class RenderCollisionSystem: public System {
    public:
        RenderCollisionSystem() {
            RequireComponent<BoxColliderComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update(SDL_Renderer* renderer) {
            auto entities = GetSystemEntities();
            for (auto entity: entities) {
                const TransformComponent& transform = entity.GetComponent<TransformComponent>();
                const BoxColliderComponent& collider = entity.GetComponent<BoxColliderComponent>();

                SDL_Rect renderRect = {
                    static_cast<int>(transform.position.x + collider.offset.x),
                    static_cast<int>(transform.position.y + collider.offset.y),
                    static_cast<int>(collider.width),
                    static_cast<int>(collider.height)
                };
                
                if (collider.colliding) {
                    spdlog::info("Drawing Red");
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); //Red
                } else {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); //Yellow
                };

                SDL_RenderDrawRect(
                    renderer,
                    &renderRect
                );

            }

        }
};

#endif