#include "camera_controller.hpp"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

namespace {

CameraController* g_cameraController = nullptr;

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    if (g_cameraController != nullptr) {
        g_cameraController->onFramebufferResize(width, height);
    }
}

void mouseCallback(GLFWwindow*, double xpos, double ypos) {
    if (g_cameraController != nullptr) {
        g_cameraController->onMouseMove(xpos, ypos);
    }
}

void scrollCallback(GLFWwindow*, double, double yoffset) {
    if (g_cameraController != nullptr) {
        g_cameraController->onScroll(yoffset);
    }
}

}  // namespace

CameraController::CameraController() = default;

void CameraController::attach(GLFWwindow* window) {
    window_ = window;
    g_cameraController = this;

    glfwSetFramebufferSizeCallback(window_, framebufferSizeCallback);
    glfwSetCursorPosCallback(window_, mouseCallback);
    glfwSetScrollCallback(window_, scrollCallback);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void CameraController::updateDeltaTime(float currentFrame) {
    deltaTime_ = currentFrame - lastFrame_;
    lastFrame_ = currentFrame;
}

void CameraController::processInput(GLFWwindow* window) {
    constexpr float kBaseSpeed = 3.5f;
    constexpr float kBoostMultiplier = 3.0f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    float cameraSpeed = kBaseSpeed * deltaTime_;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        cameraSpeed *= kBoostMultiplier;
    }

    const glm::vec3 right = glm::normalize(glm::cross(front_, up_));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position_ += cameraSpeed * front_;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position_ -= cameraSpeed * front_;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position_ -= cameraSpeed * right;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position_ += cameraSpeed * right;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position_ += cameraSpeed * up_;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        position_ -= cameraSpeed * up_;
    }
}

void CameraController::onMouseMove(double xpos, double ypos) {
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

void CameraController::onScroll(double yoffset) {
    zoom_ -= static_cast<float>(yoffset);
    zoom_ = std::clamp(zoom_, 1.0f, 90.0f);
}

void CameraController::onFramebufferResize(int width, int height) {
    glViewport(0, 0, width, height);
    if (height > 0) {
        aspectRatio_ = static_cast<float>(width) / static_cast<float>(height);
    }
}

glm::mat4 CameraController::viewMatrix() const {
    return glm::lookAt(position_, position_ + front_, up_);
}

float CameraController::zoomDegrees() const {
    return zoom_;
}

float CameraController::aspectRatio() const {
    return aspectRatio_;
}

const glm::vec3& CameraController::position() const {
    return position_;
}

const glm::vec3& CameraController::front() const {
    return front_;
}

const glm::vec3& CameraController::up() const {
    return up_;
}

void CameraController::updateFrontVector() {
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    direction.y = sin(glm::radians(pitch_));
    direction.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(direction);
}
