#pragma once

#include <glad/gl.h>

#include <cstddef>
#include <string>
#include <vector>

struct MeshBuffers {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei indexCount = 0;
};

GLuint createShaderProgramFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
GLuint createTexture(const std::string& imagePath);
MeshBuffers createMeshBuffers(
    const std::vector<float>& vertices,
    const std::vector<unsigned int>& indices,
    bool withNormalsAndTexcoords
);
MeshBuffers createMeshBuffers(
    const float* vertices,
    std::size_t vertexBytes,
    const unsigned int* indices,
    std::size_t indexBytes,
    bool withNormalsAndTexcoords
);
void destroyMeshBuffers(MeshBuffers& mesh);
