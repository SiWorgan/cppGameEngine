#ifndef KEYPRESSEDEVENT_H
#define KEYPRESSEDEVENT_H

#include <SDL2/SDL.h>

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class KeyPressedEvent: public Event {
    public:
        SDL_Keycode key;
        KeyPressedEvent(SDL_Keycode key): key(key) {}
};

#endif