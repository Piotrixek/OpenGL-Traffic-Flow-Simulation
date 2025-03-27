// Model.h
#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};
class Model {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    unsigned int textureID;
    bool hasTexture;
    float modelScale;
    Model(const std::string& objPath, bool loadTexture = true);
    ~Model();
    void Draw(unsigned int shaderProgram);
private:
    std::string directory;
    bool loadModel(const std::string& objPath, bool loadTexture);
    void setupMesh();
    unsigned int loadTextureFromFile(const std::string& path);
};
