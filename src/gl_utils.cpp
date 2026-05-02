#include "gl_utils.hpp"

#include "app_config.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {

std::string readTextFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo: " + path);
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const std::string& source) {
    const char* shaderSource = source.c_str();
    const GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_TRUE) {
        return shader;
    }

    char log[512];
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    glDeleteShader(shader);
    throw std::runtime_error(std::string("Erro ao compilar shader:\n") + log);
}

void configureVertexAttributes(bool withNormalsAndTexcoords) {
    constexpr GLsizei stride = static_cast<GLsizei>(app::kVertexStrideFloats * sizeof(float));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);

    if (!withNormalsAndTexcoords) {
        return;
    }

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

}  // namespace

GLuint createShaderProgramFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    const std::string vertexSource = readTextFile(vertexPath);
    const std::string fragmentSource = readTextFile(fragmentPath);

    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    const GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (success == GL_TRUE) {
        return program;
    }

    char log[512];
    glGetProgramInfoLog(program, sizeof(log), nullptr, log);
    glDeleteProgram(program);
    throw std::runtime_error(std::string("Erro ao linkar programa:\n") + log);
}

GLuint createTexture(const std::string& imagePath) {
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);

    if (data != nullptr) {
        const GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Erro ao carregar textura: " << imagePath << '\n';
        const unsigned char whitePixel[] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    }

    stbi_image_free(data);
    return texture;
}

MeshBuffers createMeshBuffers(
    const std::vector<float>& vertices,
    const std::vector<unsigned int>& indices,
    bool withNormalsAndTexcoords
) {
    return createMeshBuffers(
        vertices.data(),
        vertices.size() * sizeof(float),
        indices.data(),
        indices.size() * sizeof(unsigned int),
        withNormalsAndTexcoords
    );
}

MeshBuffers createMeshBuffers(
    const float* vertices,
    std::size_t vertexBytes,
    const unsigned int* indices,
    std::size_t indexBytes,
    bool withNormalsAndTexcoords
) {
    MeshBuffers mesh;
    mesh.indexCount = static_cast<GLsizei>(indexBytes / sizeof(unsigned int));

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);

    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexBytes), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indexBytes), indices, GL_STATIC_DRAW);

    configureVertexAttributes(withNormalsAndTexcoords);
    glBindVertexArray(0);

    return mesh;
}

void destroyMeshBuffers(MeshBuffers& mesh) {
    if (mesh.vao != 0) {
        glDeleteVertexArrays(1, &mesh.vao);
    }
    if (mesh.vbo != 0) {
        glDeleteBuffers(1, &mesh.vbo);
    }
    if (mesh.ebo != 0) {
        glDeleteBuffers(1, &mesh.ebo);
    }

    mesh = {};
}
