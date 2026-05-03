#pragma once

#include "engine/Camera.h"
#include "engine/GameObject.h"
#include "engine/Shader.h"

class Renderer {
public:
    void initialize();
    void renderScene(
        const Camera& camera,
        const GameObject& tube,
        const GameObject& ground,
        const GameObject& lightSphere
    );

private:
    void drawTexturedObject(
        const Shader& shader,
        const Camera& camera,
        const GameObject& object,
        const glm::vec3& lightPos
    ) const;
    void drawLightObject(const Camera& camera, const GameObject& lightSphere) const;

    Shader objectShader_;
    Shader lightShader_;
};
