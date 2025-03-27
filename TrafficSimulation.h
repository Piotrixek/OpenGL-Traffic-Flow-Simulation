// TrafficSimulation.h
#pragma once
#include <vector>
#include "Car.h"
#include <glm/glm.hpp>
#include "Model.h"
class TrafficSimulation {
public:
    std::vector<Car> cars;
    float spawnTimer;
    float spawnInterval;
    bool enableSmartAI;
    float carSpeed;
    Model* carModel;
    TrafficSimulation();
    void update(float deltaTime);
    void render(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection);
};
