// Car.cpp
#define GLM_ENABLE_EXPERIMENTAL
#include "Car.h"
#include <glm/gtx/rotate_vector.hpp>
Car::Car(glm::vec3 startPos, glm::vec3 dir, float spd, TurnType turn, int sDir)
    : position(startPos), direction(glm::normalize(dir)), speed(spd), turnType(turn), spawnDir(sDir), waiting(false), hasTurned(false) {}
void Car::update(float deltaTime) {
    if (!waiting) {
        // When near the intersection, change direction according to turn type (only once)
        if (!hasTurned) {
            if (spawnDir == 0 && position.x > -1.0f) { // from left
                if (turnType == LEFT) direction = glm::vec3(0, 0, 1);
                else if (turnType == RIGHT) direction = glm::vec3(0, 0, -1);
                hasTurned = true;
            }
            else if (spawnDir == 1 && position.x < 1.0f) { // from right
                if (turnType == LEFT) direction = glm::vec3(0, 0, -1);
                else if (turnType == RIGHT) direction = glm::vec3(0, 0, 1);
                hasTurned = true;
            }
            else if (spawnDir == 2 && position.z < 1.0f) { // from top
                if (turnType == LEFT) direction = glm::vec3(-1, 0, 0);
                else if (turnType == RIGHT) direction = glm::vec3(1, 0, 0);
                hasTurned = true;
            }
            else if (spawnDir == 3 && position.z > -1.0f) { // from bottom
                if (turnType == LEFT) direction = glm::vec3(1, 0, 0);
                else if (turnType == RIGHT) direction = glm::vec3(-1, 0, 0);
                hasTurned = true;
            }
        }
        position += direction * speed * deltaTime;
    }
}
