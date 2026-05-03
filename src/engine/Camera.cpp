#include "engine/Camera.h"

#include <algorithm>

#include <glad/gl.h>

#include <glm/gtc/matrix_transform.hpp>

namespace {

Camera* g_activeCamera = nullptr;

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    if (g_activeCamera != nullptr) {
        g_activeCamera->onFramebufferResize(width, height);
    }
}

void mouseCallback(GLFWwindow*, double xpos, double ypos) {
    if (g_activeCamera != nullptr) {
        g_activeCamera->onMouseMove(xpos, ypos);
    }
}

void scrollCallback(GLFWwindow*, double, double yoffset) {
    if (g_activeCamera != nullptr) {
        g_activeCamera->onScroll(yoffset);
    }
}

}  // namespace

Camera::Camera() = default;

void Camera::attach(GLFWwindow* window) {
    window_ = window;
    g_activeCamera = this;

    glfwSetFramebufferSizeCallback(window_, framebufferSizeCallback);
    glfwSetCursorPosCallback(window_, mouseCallback);
    glfwSetScrollCallback(window_, scrollCallback);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Camera::onMouseMove(double xpos, double ypos) {
    if (firstMouse_) {
        lastX_ = static_cast<float>(xpos);
        lastY_ = static_cast<float>(ypos);
        firstMouse_ = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX_;
    float yoffset = lastY_ - static_cast<float>(ypos);
    lastX_ = static_cast<float>(xpos);
    lastY_ = static_cast<float>(ypos);

    constexpr float kSensitivity = 0.3f;
    yaw_ += xoffset * kSensitivity;
    pitch_ += yoffset * kSensitivity;
    pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
    updateFrontVector();
}

void Camera::onScroll(double yoffset) {
    zoom_ -= static_cast<float>(yoffset);
    zoom_ = std::clamp(zoom_, 1.0f, 90.0f);
}

void Camera::onFramebufferResize(int width, int height) {
    glViewport(0, 0, width, height);
    if (height > 0) {
        aspectRatio_ = static_cast<float>(width) / static_cast<float>(height);
    }
}

void Camera::moveForward(float amount) {
    position_ += amount * front_;
}

void Camera::moveBackward(float amount) {
    position_ -= amount * front_;
}

void Camera::strafeLeft(float amount) {
    position_ -= amount * glm::normalize(glm::cross(front_, up_));
}

void Camera::strafeRight(float amount) {
    position_ += amount * glm::normalize(glm::cross(front_, up_));
}

void Camera::moveUp(float amount) {
    position_ += amount * up_;
}

void Camera::moveDown(float amount) {
    position_ -= amount * up_;
}

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::projectionMatrix() const {
    return glm::perspective(glm::radians(zoom_), aspectRatio_, 0.1f, 100.0f);
}

const glm::vec3& Camera::position() const {
    return position_;
}

void Camera::updateFrontVector() {
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    direction.y = sin(glm::radians(pitch_));
    direction.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(direction);
}
