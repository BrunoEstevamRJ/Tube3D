#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/gl.h>

struct Particle {
    glm::vec3 pos;
    glm::vec3 vel;
    float life;
};

class ParticleSystem {
public:
    void initialize(int count);
    void update(float dt);
    void draw() const;

private:
    std::vector<Particle> particles_;
    std::vector<glm::vec3> positions_; // só posição pra GPU

    GLuint VAO_ = 0;
    GLuint VBO_ = 0;
};