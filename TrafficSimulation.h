// TrafficSimulation.h
#pragma once
#include <vector>
#include "Car.h"
#include <glm/glm.hpp>
class TrafficSimulation {
public:
    std::vector<Car> cars;
    float spawnTimer;
    float spawnInterval;
    bool enableSmartAI;
    float carSpeed;
    TrafficSimulation();
    void update(float deltaTime);
    void render(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection);
};
