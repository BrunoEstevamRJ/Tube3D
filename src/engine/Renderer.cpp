#include "engine/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

void Renderer::initialize() {
    objectShader_ = Shader("shaders/object_vertex.glsl", "shaders/object_fragment.glsl");
    lightShader_ = Shader("shaders/vertex.glsl", "shaders/fragment.glsl");
}

void Renderer::renderScene(
    const Camera& camera,
    const GameObject& tube,
    const GameObject& ground,
    const GameObject& lightSphere
) {
    drawTexturedObject(objectShader_, camera, tube, lightSphere.position);
    drawTexturedObject(objectShader_, camera, ground, lightSphere.position);
    drawLightObject(camera, lightSphere);
}

void Renderer::drawTexturedObject(
    const Shader& shader,
    const Camera& camera,
    const GameObject& object,
    const glm::vec3& lightPos
) const {
    shader.use();
    shader.setMat4("model", object.modelMatrix());
    shader.setMat4("view", camera.viewMatrix());
    shader.setMat4("projection", camera.projectionMatrix());
    shader.setVec3("lightPos", lightPos);
    shader.setVec3("viewPos", camera.position());
    object.texture.bind(GL_TEXTURE0);
    shader.setInt("texture1", 0);
    object.mesh.draw();
}

void Renderer::drawLightObject(const Camera& camera, const GameObject& lightSphere) const {
    lightShader_.use();
    lightShader_.setMat4("model", lightSphere.modelMatrix());
    lightShader_.setMat4("view", camera.viewMatrix());
    lightShader_.setMat4("projection", camera.projectionMatrix());
    lightSphere.mesh.draw();
}
