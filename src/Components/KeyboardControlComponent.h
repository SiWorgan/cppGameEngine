#ifndef KEYBOARDCONTROLCOMPONENT_H
#define KEYBOARDCONTROLCOMPONENT_H

#include <glm/glm.hpp>

struct KeyboardControlComponent {
    glm::vec2 upVel;
    glm::vec2 rightVel;
    glm::vec2 downVel;
    glm::vec2 leftVel;


    KeyboardControlComponent(glm::vec2 upVel = glm::vec2(0), 
                             glm::vec2 rightVel = glm::vec2(0),
                             glm::vec2 downVel = glm::vec2(0),
                             glm::vec2 leftVel = glm::vec2(0)) {
        this->upVel = upVel;
        this->rightVel = rightVel;
        this->downVel = downVel;
        this->leftVel = leftVel;
    }
};

#endif