#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "engine/Camera.h"
#include "engine/GameObject.h"
#include "engine/Input.h"
#include "engine/Renderer.h"
#include "engine/ParticleSystem.h"

class Application {
public:
    Application();
    ~Application();

    void run();

private:
    void initialize();
    void initializeWindow();
    void initializeScene();
    void shutdown();
    void update(float currentFrame);
    void render();

    [[nodiscard]] bool isRunning() const;
    [[nodiscard]] glm::vec3 lightPosition(float timeSeconds) const;

    GLFWwindow* window_ = nullptr;
    bool initialized_ = false;
    float lastFrame_ = 0.0f;

    Camera camera_;
    Input input_;
    Renderer renderer_;

    GameObject tube_;
    GameObject ground_;
    GameObject lightSphere_;

    ParticleSystem particles_;
};
