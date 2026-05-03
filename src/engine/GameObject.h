#pragma once

#include <glm/glm.hpp>

#include "engine/Mesh.h"
#include "engine/Texture.h"

class GameObject {
public:
    GameObject() = default;
    GameObject(Mesh mesh, Texture texture);

    [[nodiscard]] glm::mat4 modelMatrix() const;

    Mesh mesh;
    Texture texture;
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
};
