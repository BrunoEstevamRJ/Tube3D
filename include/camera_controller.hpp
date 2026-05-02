#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class CameraController {
public:
    CameraController();

    void attach(GLFWwindow* window);
    void updateDeltaTime(float currentFrame);
    void processInput(GLFWwindow* window);

    void onMouseMove(double xpos, double ypos);
    void onScroll(double yoffset);
    void onFramebufferResize(int width, int height);

    [[nodiscard]] glm::mat4 viewMatrix() const;
    [[nodiscard]] float zoomDegrees() const;
    [[nodiscard]] float aspectRatio() const;

    [[nodiscard]] const glm::vec3& position() const;
    [[nodiscard]] const glm::vec3& front() const;
    [[nodiscard]] const glm::vec3& up() const;

private:
    void updateFrontVector();

    GLFWwindow* window_ = nullptr;
    glm::vec3 position_{0.0f, 0.0f, 5.0f};
    glm::vec3 front_{0.0f, 0.0f, -1.0f};
    glm::vec3 up_{0.0f, 1.0f, 0.0f};

    float yaw_ = -90.0f;
    float pitch_ = 0.0f;
    float zoom_ = 45.0f;
    float deltaTime_ = 0.0f;
    float lastFrame_ = 0.0f;
    float lastX_ = 400.0f;
    float lastY_ = 300.0f;
    float aspectRatio_ = 800.0f / 600.0f;
    bool firstMouse_ = true;
};
