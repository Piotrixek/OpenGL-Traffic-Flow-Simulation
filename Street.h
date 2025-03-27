// Street.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
class Street {
public:
    float roadWidth;
    float roadLength;
    Street(float width = 2.0f, float length = 20.0f);
    void regenerate(float width, float length);
    void render(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection);
private:
    unsigned int VAO, VBO;
};
