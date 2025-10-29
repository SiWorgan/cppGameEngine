#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"

class AnimationSystem: public System {
    public:
        AnimationSystem() {
            RequireComponent<SpriteComponent>();
            RequireComponent<AnimationComponent>();
        }

        void Update(double deltaTime) {
            for (auto entity: GetSystemEntities()) {
                auto& animation = entity.GetComponent<AnimationComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();

                animation.elapsedTime += deltaTime;

                animation.currentFrame = static_cast<int>(animation.elapsedTime * animation.frameSpeedRate) % animation.numFrames; 
                sprite.srcRect.x = animation.currentFrame * sprite.width;
            }
        }
};

#endif