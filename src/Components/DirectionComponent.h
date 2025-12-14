#ifndef DIRECTIONCOMPONENT_H
#define DIRECTIONCOMPONENT_H

#include <glm/glm.hpp>

struct DirectionComponent {
    glm::vec2 direction;

    DirectionComponent(glm::vec2 direction = glm::vec2(1.0, 0.0)) {
        this->direction = direction;
    }
};

#endif