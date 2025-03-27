// Street.cpp
#define GLM_ENABLE_EXPERIMENTAL
#include "Street.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <glad/glad.h>
Street::Street(float width, float length) : roadWidth(width), roadLength(length), VAO(0), VBO(0) {
    regenerate(width, length);
}
void Street::regenerate(float width, float length) {
    roadWidth = width;
    roadLength = length;
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
    float halfLength = roadLength * 0.5f;
    float halfWidth = roadWidth * 0.5f;
    // Two roads forming a cross: horizontal and vertical
    float horizontalRoad[] = {
        -halfLength, 0.0f, -halfWidth,
         halfLength, 0.0f, -halfWidth,
         halfLength, 0.0f,  halfWidth,
         halfLength, 0.0f,  halfWidth,
        -halfLength, 0.0f,  halfWidth,
        -halfLength, 0.0f, -halfWidth,
    };
    float verticalRoad[] = {
        -halfWidth, 0.0f, -halfLength,
         halfWidth, 0.0f, -halfLength,
         halfWidth, 0.0f,  halfLength,
         halfWidth, 0.0f,  halfLength,
        -halfWidth, 0.0f,  halfLength,
        -halfWidth, 0.0f, -halfLength,
    };
    std::vector<float> vertices;
    vertices.insert(vertices.end(), horizontalRoad, horizontalRoad + 18);
    vertices.insert(vertices.end(), verticalRoad, verticalRoad + 18);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}
void Street::render(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 12);
}
