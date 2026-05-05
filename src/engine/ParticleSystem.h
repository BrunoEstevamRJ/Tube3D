#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/gl.h>

#include "engine/Camera.h"
#include "engine/Shader.h"

struct Particle {
    glm::vec3 pos;
    glm::vec3 vel;
    float life;
    float maxLife;
    float size;
};

class ParticleSystem {
public:
    void initialize(int count);
    void setEmitterPosition(const glm::vec3& position);
    void update(float dt);
    void draw(const Camera& camera) const;

private:
    std::vector<Particle> particles_;
    std::vector<float> renderData_;

    GLuint VAO_ = 0;
    GLuint VBO_ = 0;
    Shader shader_;
    glm::vec3 emitterPosition_{0.0f, 2.0f, 0.0f};
};
