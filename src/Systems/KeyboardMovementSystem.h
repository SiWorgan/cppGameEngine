#ifndef KEYBOARDMOVEMENTSYSTEM_H
#define KEYBOARDMOVEMENTSYSTEM_H

#include "spdlog/spdlog.h"

#include "../ECS/ECS.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/DirectionComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

class KeyboardMovementSystem: public System {
    public:
        KeyboardMovementSystem() {
            RequireComponent<KeyboardControlComponent>();
            RequireComponent<RigidBodyComponent>();
            RequireComponent<SpriteComponent>();
            RequireComponent<DirectionComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::onPress);
        }

        void onPress(KeyPressedEvent& event) {
            for (auto entity: GetSystemEntities()) {
                const auto keyboardcontrol = entity.GetComponent<KeyboardControlComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();
                auto& rigidbody = entity.GetComponent<RigidBodyComponent>();
                auto& direction = entity.GetComponent<DirectionComponent>();

                switch (event.key) {
                    case SDLK_UP:
                        rigidbody.velocity = keyboardcontrol.upVel;
                        sprite.srcRect.y = sprite.height * 0;
                        direction.direction = glm::vec2(0, -1);
                        break;
                    case SDLK_RIGHT:
                        rigidbody.velocity = keyboardcontrol.rightVel;
                        sprite.srcRect.y = sprite.height * 1;
                        direction.direction = glm::vec2(1, 0);
                        break;
                    case SDLK_DOWN:
                        rigidbody.velocity = keyboardcontrol.downVel;
                        sprite.srcRect.y = sprite.height * 2;
                        direction.direction = glm::vec2(0, 1);
                        break;
                    case SDLK_LEFT:
                        rigidbody.velocity = keyboardcontrol.leftVel;
                        sprite.srcRect.y = sprite.height * 3;
                        direction.direction = glm::vec2(-1, 0);
                        break; 
                }
            }
        }

        void Update(){

        }
};

#endif