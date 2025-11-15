#ifndef RENDERCOLLISIONSYSTEM_H
#define RENDERCOLLISIONSYSTEM_H

#include <SDL2/SDL.h>
#include "spdlog/spdlog.h"

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

class RenderCollisionSystem: public System {
    public:
        RenderCollisionSystem() {
            RequireComponent<BoxColliderComponent>();
            RequireComponent<TransformComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<CollisionEvent>(this, &RenderCollisionSystem::onCollision);
        }

        void onCollision(CollisionEvent& event) {
            drawColour[1] = 0; //Red
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
                
                SDL_SetRenderDrawColor(renderer, drawColour[0], drawColour[1], drawColour[2], drawColour[3]); //Yellow

                SDL_RenderDrawRect(
                    renderer,
                    &renderRect
                );

            }
            //Set back to Yellow
            drawColour[1] = 255;
        }

        private:
            int drawColour[4] = {255, 255, 0, 255}; //Yellow


};

#endif