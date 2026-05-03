#include "engine/Application.h"

#include <iostream>
#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>

#include "app_config.hpp"
#include "engine/Mesh.h"
#include "engine/Texture.h"

Application::Application() = default;

Application::~Application() {
    shutdown();
}

void Application::run() {
    try {
        initialize();

        while (isRunning()) {
            const float currentFrame = static_cast<float>(glfwGetTime());
            update(currentFrame);
            render();
            glfwSwapBuffers(window_);
            glfwPollEvents();
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
    }
}

void Application::initialize() {
    if (initialized_) {
        return;
    }

    initializeWindow();
    renderer_.initialize();
    initializeScene();
    initialized_ = true;
}

void Application::initializeWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("Erro ao inicializar GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(
        static_cast<int>(app::kWindowWidth),
        static_cast<int>(app::kWindowHeight),
        app::kWindowTitle,
        nullptr,
        nullptr
    );

    if (window_ == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Erro ao criar janela GLFW");
    }

    glfwMakeContextCurrent(window_);
    if (!gladLoadGL(glfwGetProcAddress)) {
        throw std::runtime_error("Erro ao carregar OpenGL");
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, app::kWindowWidth, app::kWindowHeight);
    camera_.attach(window_);
}

void Application::initializeScene() {    
    tube_ = GameObject(
        Mesh::createTube(
            app::kTubeInnerRadius,
            app::kTubeOuterRadius,
            app::kTubeHeight,
            app::kTubeSegments
        ),
        Texture("wall.jpg")
    );

    ground_ = GameObject(
        Mesh::createFromRaw(
            app::kGroundVertices.data(),
            app::kGroundVertices.size() * sizeof(float),
            app::kGroundIndices.data(),
            app::kGroundIndices.size() * sizeof(unsigned int),
            true
        ),
        Texture("ground.jpg")
    );

    lightSphere_ = GameObject(
        Mesh::createSphere(
            app::kLightSphereRadius,
            app::kLightSphereSectors,
            app::kLightSphereStacks
        ),
        Texture()
    );

    particles_.initialize(500);
}

void Application::shutdown() {
    if (window_ != nullptr) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }

    if (initialized_) {
        glfwTerminate();
        initialized_ = false;
    }
}

void Application::update(float currentFrame) {
    const float deltaTime = currentFrame - lastFrame_;
    lastFrame_ = currentFrame;

    input_.process(window_, camera_, deltaTime);
    particles_.update(deltaTime);
    lightSphere_.position = lightPosition(currentFrame);
}

void Application::render() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer_.renderScene(camera_, tube_, ground_, lightSphere_);

    particles_.draw();
}

bool Application::isRunning() const {
    return window_ != nullptr && !glfwWindowShouldClose(window_);
}

glm::vec3 Application::lightPosition(float timeSeconds) const {
    return {
        app::kLightOrbitRadius * sinf(timeSeconds),
        2.0f,
        app::kLightOrbitRadius * cosf(timeSeconds),
    };
}


//update = CPU (lógica)
//draw   = GPU (render)