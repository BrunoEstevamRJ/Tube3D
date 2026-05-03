#include "engine/GameObject.h"

#include <glm/gtc/matrix_transform.hpp>

GameObject::GameObject(Mesh meshValue, Texture textureValue)
    : mesh(std::move(meshValue)), texture(std::move(textureValue)) {}

glm::mat4 GameObject::modelMatrix() const {
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}
