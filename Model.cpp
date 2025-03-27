// Model.cpp
#include "Model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Model::Model(const std::string& objPath, bool loadTexture)
    : VAO(0), VBO(0), EBO(0), textureID(0), hasTexture(false), modelScale(0.1f) {
    size_t pos = objPath.find_last_of("/\\");
    directory = (pos != std::string::npos) ? objPath.substr(0, pos) : ".";
    if (loadModel(objPath, loadTexture))
        setupMesh();
}

Model::~Model() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (textureID) glDeleteTextures(1, &textureID);
}

bool Model::loadModel(const std::string& objPath, bool loadTexture) {
    std::ifstream inFile(objPath);
    if (!inFile) {
        std::cerr << "Cannot open OBJ file: " << objPath << std::endl;
        return false;
    }
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_texcoords;
    std::vector<glm::vec3> temp_normals;
    std::string line;
    std::string mtlFile, texturePath;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "mtllib") {
            iss >> mtlFile;
            std::string mtlPath = directory + "/" + mtlFile;
            std::ifstream mtlStream(mtlPath);
            if (mtlStream) {
                std::string mtlLine;
                while (std::getline(mtlStream, mtlLine)) {
                    std::istringstream mtlIss(mtlLine);
                    std::string mtlPrefix;
                    mtlIss >> mtlPrefix;
                    if (mtlPrefix == "map_Kd") {
                        std::getline(mtlIss, texturePath);
                        texturePath.erase(0, texturePath.find_first_not_of(" \t"));
                        break;
                    }
                }
                mtlStream.close();
            }
        }
        else if (prefix == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        }
        else if (prefix == "vt") {
            glm::vec2 tex;
            iss >> tex.x >> tex.y;
            temp_texcoords.push_back(tex);
        }
        else if (prefix == "vn") {
            glm::vec3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            temp_normals.push_back(norm);
        }
        else if (prefix == "f") {
            std::string vertexStr;
            while (iss >> vertexStr) {
                std::istringstream viss(vertexStr);
                std::string posIndexStr, texIndexStr, normIndexStr;
                std::getline(viss, posIndexStr, '/');
                std::getline(viss, texIndexStr, '/');
                std::getline(viss, normIndexStr, '/');
                int posIndex = std::stoi(posIndexStr);
                int texIndex = texIndexStr.empty() ? 0 : std::stoi(texIndexStr);
                int normIndex = normIndexStr.empty() ? 0 : std::stoi(normIndexStr);
                Vertex vertex;
                vertex.position = temp_positions[posIndex - 1];
                vertex.texCoord = texIndex ? temp_texcoords[texIndex - 1] : glm::vec2(0.0f);
                vertex.normal = normIndex ? temp_normals[normIndex - 1] : glm::vec3(0.0f);
                vertices.push_back(vertex);
                indices.push_back((unsigned int)indices.size());
            }
        }
    }
    inFile.close();
    if (loadTexture && !texturePath.empty()) {
        std::string fullTexPath;
        if (texturePath.find(":") != std::string::npos || texturePath[0] == '/' || texturePath[0] == '\\')
            fullTexPath = texturePath;
        else
            fullTexPath = directory + "/" + texturePath;
        textureID = loadTextureFromFile(fullTexPath);
        if (textureID)
            hasTexture = true;
    }
    return true;
}

void Model::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glBindVertexArray(0);
}

unsigned int Model::loadTextureFromFile(const std::string& path) {
    if (path.empty()) {
        std::cerr << "Empty texture path provided." << std::endl;
        return 0;
    }
    std::ifstream file(path);
    if (!file.good()) {
        std::cerr << "Texture file does not exist: " << path << std::endl;
        return 0;
    }
    file.close();
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        unsigned int texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
        return texID;
    }
    else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }
}

void Model::Draw(unsigned int shaderProgram) {
    if (hasTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        int texLoc = glGetUniformLocation(shaderProgram, "texSampler");
        glUniform1i(texLoc, 0);
    }
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
