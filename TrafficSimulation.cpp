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
static unsigned int cubeVAO = 0, cubeVBO = 0;
TrafficSimulation::TrafficSimulation() : spawnTimer(0.0f), spawnInterval(2.0f), enableSmartAI(true), carSpeed(3.0f) {
    if (cubeVAO == 0) {
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
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
    for (auto& car : cars) {
        car.waiting = false;
        if (enableSmartAI) {
            if (glm::all(glm::epsilonEqual(car.direction, glm::vec3(1, 0, 0), 0.1f)) &&
                car.position.x > -2.0f && car.position.x < 0.0f) {
                for (auto& other : cars) {
                    if (glm::all(glm::epsilonEqual(other.direction, glm::vec3(0, 0, 1), 0.1f)) &&
                        other.position.z > -2.0f && other.position.z < 0.0f) {
                        car.waiting = true;
                    }
                }
            }
        }
        car.update(deltaTime);
    }
    cars.erase(std::remove_if(cars.begin(), cars.end(), [](const Car& car) {
        return (glm::length(car.position) > 30.0f);
        }), cars.end());
}
void TrafficSimulation::render(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);
    for (auto& car : cars) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), car.position);
        model = glm::scale(model, glm::vec3(1.0f, 0.5f, 0.5f));
        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}
