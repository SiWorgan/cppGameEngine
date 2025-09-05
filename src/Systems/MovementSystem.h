#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

class MovementSystem: public System {

    public:
        MovementSystem() {
            // TODO: RequireComponent<TransformComponent>();
            //       RequireComponent<...>();
        }

        void Update() {
            // TODO: Update entity position based on its velocity
            for (auto entity: GetEntities()) {

            }
        }
};

#endif