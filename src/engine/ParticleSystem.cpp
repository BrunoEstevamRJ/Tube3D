#include "engine/ParticleSystem.h"

#include <cmath>
#include <cstdlib>

namespace {

constexpr float kPi = 3.14159265359f;

float randomFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float randomRange(float minValue, float maxValue) {
    return minValue + (maxValue - minValue) * randomFloat();
}

glm::vec3 randomUnitVector() {
    const float theta = randomFloat() * 2.0f * kPi;
    const float phi = std::acos(2.0f * randomFloat() - 1.0f);

    return glm::vec3(
        std::sin(phi) * std::cos(theta),
        std::sin(phi) * std::sin(theta),
        std::cos(phi)
    );
}

glm::vec3 randomDiscDirection(float verticalSpread) {
    const float theta = randomFloat() * 2.0f * kPi;
    glm::vec3 dir(
        std::cos(theta),
        randomRange(-verticalSpread, verticalSpread),
        std::sin(theta)
    );
    return glm::normalize(dir);
}

glm::vec3 tangentialDirection(const glm::vec3& dir) {
    glm::vec3 tangent = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), dir);
    if (glm::length(tangent) < 0.001f) {
        tangent = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), dir);
    }
    return glm::normalize(tangent);
}

void resetParticle(Particle& p, const glm::vec3& emitterPosition) {
    const float profile = randomFloat();
    glm::vec3 dir(0.0f);
    glm::vec3 velocity(0.0f);

    if (profile < 0.6f) {
        dir = randomDiscDirection(0.18f);
        velocity = dir * randomRange(3.8f, 6.0f);
        velocity += tangentialDirection(dir) * randomRange(0.6f, 1.5f);
        p.size = randomRange(6.0f, 10.0f);
        p.life = randomRange(1.0f, 1.6f);
    } else if (profile < 0.85f) {
        dir = glm::normalize(glm::mix(randomUnitVector(), randomDiscDirection(0.35f), 0.45f));
        velocity = dir * randomRange(2.2f, 4.0f);
        p.size = randomRange(4.0f, 7.0f);
        p.life = randomRange(0.8f, 1.3f);
    } else {
        const float sign = randomFloat() < 0.5f ? -1.0f : 1.0f;
        dir = glm::normalize(glm::vec3(
            randomRange(-0.25f, 0.25f),
            sign * randomRange(0.7f, 1.0f),
            randomRange(-0.25f, 0.25f)
        ));
        velocity = dir * randomRange(3.0f, 5.2f);
        p.size = randomRange(3.0f, 5.5f);
        p.life = randomRange(0.7f, 1.1f);
    }

    p.pos = emitterPosition + dir * randomRange(0.02f, 0.18f);
    p.vel = velocity;
    p.maxLife = p.life;
}

} // namespace

void ParticleSystem::initialize(int count) {
    particles_.resize(count);
    renderData_.reserve(static_cast<size_t>(count) * 5);

    for (auto& p : particles_) {
        resetParticle(p, emitterPosition_);
    }

    shader_.loadFromFiles("shaders/particle_vertex.glsl", "shaders/particle_fragment.glsl");
    glEnable(GL_PROGRAM_POINT_SIZE);

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(particles_.size() * 5 * sizeof(float)),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void ParticleSystem::setEmitterPosition(const glm::vec3& position) {
    emitterPosition_ = position;
}

void ParticleSystem::update(float dt) {
    renderData_.clear();

    for (auto& p : particles_) {
        if (p.life > 0.0f) {
            const glm::vec3 offset = p.pos - emitterPosition_;
            const float distance = glm::length(offset);
            const glm::vec3 radialDir = distance > 0.0001f ? offset / distance : randomUnitVector();
            const glm::vec3 inwardPull = -radialDir * distance * 0.55f;
            const glm::vec3 swirl = tangentialDirection(radialDir) * (1.1f / (1.0f + distance));

            p.vel += (inwardPull + swirl) * dt;
            p.vel *= 0.985f;

            p.pos += p.vel * dt;
            p.life -= dt;

            const float lifeRatio = p.life / p.maxLife;
            renderData_.push_back(p.pos.x);
            renderData_.push_back(p.pos.y);
            renderData_.push_back(p.pos.z);
            renderData_.push_back(glm::clamp(lifeRatio, 0.0f, 1.0f));
            renderData_.push_back(p.size);
        } else {
            resetParticle(p, emitterPosition_);

            renderData_.push_back(p.pos.x);
            renderData_.push_back(p.pos.y);
            renderData_.push_back(p.pos.z);
            renderData_.push_back(1.0f);
            renderData_.push_back(p.size);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        static_cast<GLsizeiptr>(renderData_.size() * sizeof(float)),
        renderData_.data()
    );
}

void ParticleSystem::draw(const Camera& camera) const {
    shader_.use();
    shader_.setMat4("view", camera.viewMatrix());
    shader_.setMat4("projection", camera.projectionMatrix());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    glBindVertexArray(VAO_);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(renderData_.size() / 5));
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
