#pragma once

struct GLFWwindow;

class Camera;

class Input {
public:
    void process(GLFWwindow* window, Camera& camera, float deltaTime) const;
};
