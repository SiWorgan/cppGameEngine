#ifndef KEYBOARDMOVEMENTSYSTEM_H
#define KEYBOARDMOVEMENTSYSTEM_H

#include "spdlog/spdlog.h"

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

class KeyboardMovementSystem: public System {
    public:
        KeyboardMovementSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::onPress);
        }

        void onPress(KeyPressedEvent& event) {
            std::string keyCode = std::to_string(event.key);
            std::string keySymbol(1, event.key);
            spdlog::info("Key Pressed! [" + keyCode + "] " + keySymbol);
        }

        void Update(){

        }
};

#endif