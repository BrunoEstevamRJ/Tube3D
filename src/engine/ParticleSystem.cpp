#include "engine/ParticleSystem.h"

#include <cmath>
#include <cstdlib>

void ParticleSystem::initialize(int count) {
    particles_.resize(count);

    for (auto& p : particles_) {
        float angle = (rand() % 1000 / 1000.0f) * 6.28318f;
        float radius = 2.2f;

        p.pos = glm::vec3(
            std::cos(angle) * radius,
            (rand() % 100 / 100.0f) * 3.0f - 1.0f,
            std::sin(angle) * radius
        );

        p.vel = glm::vec3(
            -std::sin(angle) * 0.3f,
            0.4f,
            std::cos(angle) * 0.3f
        );

        p.life = 1.0f;
    }

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, particles_.size()*sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}


void ParticleSystem::update(float dt) {
    positions_.clear();

    for (auto& p : particles_) {
        if (p.life > 0.0f) {
            p.pos += p.vel * dt * 10.0f;
            p.life -= dt;
            positions_.push_back(p.pos);
        } else {
            float angle = (rand() % 1000 / 1000.0f) * 6.28318f;
            float radius = 2.2f;

            p.pos = glm::vec3(
                std::cos(angle) * radius,
                (rand() % 100 / 100.0f) * 3.0f - 1.0f,
                std::sin(angle) * radius
            );

            p.vel = glm::vec3(
                -std::sin(angle) * 0.3f,
                0.4f,
                std::cos(angle) * 0.3f
            );

            p.life = 2.0f;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positions_.size() * sizeof(glm::vec3), positions_.data());
}

void ParticleSystem::draw() const {
    glBindVertexArray(VAO_);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(positions_.size()));
    glBindVertexArray(0);
}
