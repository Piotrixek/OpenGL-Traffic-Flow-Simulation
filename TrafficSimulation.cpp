// TrafficSimulation.cpp
#define GLFW_INCLUDE_NONE
#include "TrafficSimulation.h"
#include "Logger.h"
#include "Globals.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <algorithm>
#include <cstdlib>
#include <glm/gtc/epsilon.hpp>

const float collisionDistance = 1.0f;
const float explosionDuration = 1.0f;

TrafficSimulation::TrafficSimulation()
    : spawnTimer(0.0f), spawnInterval(2.0f), enableSmartAI(true), carSpeed(3.0f), carModel(nullptr)
{
}

void TrafficSimulation::update(float deltaTime) {
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.0f;
        int spawnDir = rand() % 4;
        int turn = rand() % 3;
        TurnType turnType = static_cast<TurnType>(turn);
        glm::vec3 pos, dir;
        if (spawnDir == 0) { pos = glm::vec3(-12.0f, 0.0f, 0.0f); dir = glm::vec3(1.0f, 0.0f, 0.0f); }
        else if (spawnDir == 1) { pos = glm::vec3(12.0f, 0.0f, 0.0f); dir = glm::vec3(-1.0f, 0.0f, 0.0f); }
        else if (spawnDir == 2) { pos = glm::vec3(0.0f, 0.0f, 12.0f); dir = glm::vec3(0.0f, 0.0f, -1.0f); }
        else { pos = glm::vec3(0.0f, 0.0f, -12.0f); dir = glm::vec3(0.0f, 0.0f, 1.0f); }
        Car newCar(pos, dir, carSpeed, turnType, spawnDir);
        cars.push_back(newCar);
        logger.addLog("Spawned new car");
    }
    for (size_t i = 0; i < cars.size(); i++) {
        for (size_t j = i + 1; j < cars.size(); j++) {
            if (!cars[i].exploded && !cars[j].exploded) {
                if (glm::distance(cars[i].position, cars[j].position) < collisionDistance) {
                    cars[i].exploded = true;
                    cars[j].exploded = true;
                    logger.addLog("Collision detected - explosion triggered");
                }
            }
        }
    }
    for (auto& car : cars) {
        car.waiting = false;
        if (enableSmartAI) {
            if (glm::all(glm::epsilonEqual(car.direction, glm::vec3(1, 0, 0), 0.1f)) &&
                car.position.x > -2.0f && car.position.x < 0.0f) {
                for (auto& other : cars) {
                    if (glm::all(glm::epsilonEqual(other.direction, glm::vec3(0, 0, 1), 0.1f)) &&
                        other.position.z > -2.0f && other.position.z < 0.0f)
                    {
                        car.waiting = true;
                    }
                }
            }
        }
        car.update(deltaTime);
    }
    cars.erase(std::remove_if(cars.begin(), cars.end(), [](const Car& car) {
        return (car.exploded && car.explosionTimer > explosionDuration) || (glm::length(car.position) > 30.0f);
        }), cars.end());
}

void TrafficSimulation::render(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
    // Assumes the car shader (car.vert/car.frag) is already active
    for (auto& car : cars) {
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), car.position);
        float scaleFactor = 1.0f;
        bool exploding = false;
        float explosionProgress = 0.0f;
        if (car.exploded) {
            scaleFactor = 1.0f + 2.0f * (car.explosionTimer / explosionDuration);
            explosionProgress = car.explosionTimer / explosionDuration;
            exploding = true;
        }
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
        // Apply the model's internal scale factor:
        if (carModel)
            modelMatrix = modelMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(carModel->modelScale));

        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        int explodingLoc = glGetUniformLocation(shaderProgram, "exploding");
        int explosionProgressLoc = glGetUniformLocation(shaderProgram, "explosionProgress");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
        glUniform1i(explodingLoc, exploding);
        glUniform1f(explosionProgressLoc, explosionProgress);
        if (carModel)
            carModel->Draw(shaderProgram);
    }
}
