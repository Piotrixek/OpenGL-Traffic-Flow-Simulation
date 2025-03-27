// Car.h
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
enum TurnType { STRAIGHT, LEFT, RIGHT };
class Car {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
    TurnType turnType;
    int spawnDir; // 0: left, 1: right, 2: top, 3: bottom
    bool waiting;
    bool hasTurned;
    bool exploded;
    float explosionTimer;
    Car(glm::vec3 startPos, glm::vec3 dir, float spd, TurnType turn, int spawnDir);
    void update(float deltaTime);
};
