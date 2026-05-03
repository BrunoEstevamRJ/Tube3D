#pragma once

#include <glad/gl.h>

#include <cstddef>
#include <vector>

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    static Mesh createSphere(float radius, int sectorCount, int stackCount);
    static Mesh createTube(float innerRadius, float outerRadius, float height, int segments);
    static Mesh createFromRaw(
        const float* vertices,
        std::size_t vertexBytes,
        const unsigned int* indices,
        std::size_t indexBytes,
        bool withNormalsAndTexcoords
    );

    void draw() const;

private:
    void upload(
        const float* vertices,
        std::size_t vertexBytes,
        const unsigned int* indices,
        std::size_t indexBytes,
        bool withNormalsAndTexcoords
    );

    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;
    GLsizei indexCount_ = 0;
};
