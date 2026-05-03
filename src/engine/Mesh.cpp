#include "engine/Mesh.h"

#include <cmath>

#include "app_config.hpp"

namespace {

constexpr float kPi = 3.14159265359f;

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

Mesh::~Mesh() {
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
    }
}

Mesh::Mesh(Mesh&& other) noexcept
    : vao_(other.vao_), vbo_(other.vbo_), ebo_(other.ebo_), indexCount_(other.indexCount_) {
    other.vao_ = 0;
    other.vbo_ = 0;
    other.ebo_ = 0;
    other.indexCount_ = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        if (vao_ != 0) {
            glDeleteVertexArrays(1, &vao_);
        }
        if (vbo_ != 0) {
            glDeleteBuffers(1, &vbo_);
        }
        if (ebo_ != 0) {
            glDeleteBuffers(1, &ebo_);
        }

        vao_ = other.vao_;
        vbo_ = other.vbo_;
        ebo_ = other.ebo_;
        indexCount_ = other.indexCount_;

        other.vao_ = 0;
        other.vbo_ = 0;
        other.ebo_ = 0;
        other.indexCount_ = 0;
    }
    return *this;
}

Mesh Mesh::createSphere(float radius, int sectorCount, int stackCount) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int stack = 0; stack <= stackCount; ++stack) {
        const float stackAngle = (kPi / 2.0f) - (static_cast<float>(stack) * kPi / static_cast<float>(stackCount));
        const float xy = radius * cosf(stackAngle);
        const float z = radius * sinf(stackAngle);

        for (int sector = 0; sector <= sectorCount; ++sector) {
            const float sectorAngle = static_cast<float>(sector) * 2.0f * kPi / static_cast<float>(sectorCount);
            const float x = xy * cosf(sectorAngle);
            const float y = xy * sinf(sectorAngle);

            vertices.insert(vertices.end(), {
                x, y, z,
                x / radius, y / radius, z / radius,
                static_cast<float>(sector) / static_cast<float>(sectorCount),
                static_cast<float>(stack) / static_cast<float>(stackCount),
            });
        }
    }

    for (int stack = 0; stack < stackCount; ++stack) {
        int current = stack * (sectorCount + 1);
        int next = current + sectorCount + 1;

        for (int sector = 0; sector < sectorCount; ++sector, ++current, ++next) {
            if (stack != 0) {
                indices.insert(indices.end(), {
                    static_cast<unsigned int>(current),
                    static_cast<unsigned int>(next),
                    static_cast<unsigned int>(current + 1),
                });
            }

            if (stack != (stackCount - 1)) {
                indices.insert(indices.end(), {
                    static_cast<unsigned int>(current + 1),
                    static_cast<unsigned int>(next),
                    static_cast<unsigned int>(next + 1),
                });
            }
        }
    }

    Mesh mesh;
    mesh.upload(
        vertices.data(),
        vertices.size() * sizeof(float),
        indices.data(),
        indices.size() * sizeof(unsigned int),
        false
    );
    return mesh;
}

Mesh Mesh::createTube(float innerRadius, float outerRadius, float height, int segments) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int segment = 0; segment <= segments; ++segment) {
        const float theta = 2.0f * kPi * static_cast<float>(segment) / static_cast<float>(segments);
        const float cosTheta = cosf(theta);
        const float sinTheta = sinf(theta);
        const float u = static_cast<float>(segment) / static_cast<float>(segments);

        vertices.insert(vertices.end(), {
            outerRadius * cosTheta, outerRadius * sinTheta, 0.0f,
            cosTheta, sinTheta, 0.0f,
            u, 0.0f,

            outerRadius * cosTheta, outerRadius * sinTheta, height,
            cosTheta, sinTheta, 0.0f,
            u, 1.0f,

            innerRadius * cosTheta, innerRadius * sinTheta, 0.0f,
            -cosTheta, -sinTheta, 0.0f,
            u, 0.0f,

            innerRadius * cosTheta, innerRadius * sinTheta, height,
            -cosTheta, -sinTheta, 0.0f,
            u, 1.0f,
        });
    }

    for (int segment = 0; segment < segments; ++segment) {
        const unsigned int base = static_cast<unsigned int>(segment * 4);
        const unsigned int next = static_cast<unsigned int>((segment + 1) * 4);

        indices.insert(indices.end(), {
            base, base + 1, next + 1,
            base, next + 1, next,

            base + 2, next + 2, next + 3,
            base + 2, next + 3, base + 3,

            base + 1, next + 1, next + 3,
            base + 1, next + 3, base + 3,

            base, base + 2, next + 2,
            base, next + 2, next,
        });
    }

    Mesh mesh;
    mesh.upload(
        vertices.data(),
        vertices.size() * sizeof(float),
        indices.data(),
        indices.size() * sizeof(unsigned int),
        true
    );
    return mesh;
}

Mesh Mesh::createFromRaw(
    const float* vertices,
    std::size_t vertexBytes,
    const unsigned int* indices,
    std::size_t indexBytes,
    bool withNormalsAndTexcoords
) {
    Mesh mesh;
    mesh.upload(vertices, vertexBytes, indices, indexBytes, withNormalsAndTexcoords);
    return mesh;
}

void Mesh::draw() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::upload(
    const float* vertices,
    std::size_t vertexBytes,
    const unsigned int* indices,
    std::size_t indexBytes,
    bool withNormalsAndTexcoords
) {
    indexCount_ = static_cast<GLsizei>(indexBytes / sizeof(unsigned int));

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexBytes), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indexBytes), indices, GL_STATIC_DRAW);

    configureVertexAttributes(withNormalsAndTexcoords);
    glBindVertexArray(0);
}
