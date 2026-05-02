#include "geometry.hpp"

#include <cmath>

namespace {

constexpr float kPi = 3.14159265359f;

}

MeshData generateSphere(float radius, int sectorCount, int stackCount) {
    MeshData mesh;

    for (int stack = 0; stack <= stackCount; ++stack) {
        const float stackAngle = (kPi / 2.0f) - (static_cast<float>(stack) * kPi / static_cast<float>(stackCount));
        const float xy = radius * cosf(stackAngle);
        const float z = radius * sinf(stackAngle);

        for (int sector = 0; sector <= sectorCount; ++sector) {
            const float sectorAngle = static_cast<float>(sector) * 2.0f * kPi / static_cast<float>(sectorCount);
            const float x = xy * cosf(sectorAngle);
            const float y = xy * sinf(sectorAngle);

            mesh.vertices.insert(mesh.vertices.end(), {
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
                mesh.indices.insert(mesh.indices.end(), {
                    static_cast<unsigned int>(current),
                    static_cast<unsigned int>(next),
                    static_cast<unsigned int>(current + 1),
                });
            }

            if (stack != (stackCount - 1)) {
                mesh.indices.insert(mesh.indices.end(), {
                    static_cast<unsigned int>(current + 1),
                    static_cast<unsigned int>(next),
                    static_cast<unsigned int>(next + 1),
                });
            }
        }
    }

    return mesh;
}

MeshData generateTube(float innerRadius, float outerRadius, float height, int segments) {
    MeshData mesh;

    for (int segment = 0; segment <= segments; ++segment) {
        const float theta = 2.0f * kPi * static_cast<float>(segment) / static_cast<float>(segments);
        const float cosTheta = cosf(theta);
        const float sinTheta = sinf(theta);
        const float u = static_cast<float>(segment) / static_cast<float>(segments);

        mesh.vertices.insert(mesh.vertices.end(), {
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

        mesh.indices.insert(mesh.indices.end(), {
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

    return mesh;
}
