#include <iostream>
#include <stdexcept>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "app_config.hpp"
#include "camera_controller.hpp"
#include "geometry.hpp"
#include "gl_utils.hpp"

namespace {

GLFWwindow* createWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("Erro ao inicializar GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        static_cast<int>(app::kWindowWidth),
        static_cast<int>(app::kWindowHeight),
        app::kWindowTitle,
        nullptr,
        nullptr
    );

    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Erro ao criar janela GLFW");
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Erro ao carregar OpenGL");
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, app::kWindowWidth, app::kWindowHeight);
    return window;
}

glm::vec3 orbitingLightPosition(float timeSeconds) {
    return {
        app::kLightOrbitRadius * sinf(timeSeconds),
        2.0f,
        app::kLightOrbitRadius * cosf(timeSeconds),
    };
}

void setCommonUniforms(
    GLuint program,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& lightPos,
    const glm::vec3& viewPos
) {
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(program, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, glm::value_ptr(viewPos));
}

void drawTexturedMesh(
    GLuint program,
    const MeshBuffers& mesh,
    GLuint texture,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& lightPos,
    const glm::vec3& viewPos
) {
    glUseProgram(program);
    setCommonUniforms(program, model, view, projection, lightPos, viewPos);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(program, "texture1"), 0);
    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void drawLightMesh(
    GLuint program,
    const MeshBuffers& mesh,
    const glm::vec3& lightPos,
    const glm::mat4& view,
    const glm::mat4& projection
) {
    glUseProgram(program);

    glm::mat4 model(1.0f);
    model = glm::translate(model, lightPos);

    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

}  // namespace

int main() {
    GLFWwindow* window = nullptr;

    try {
        window = createWindow();

        CameraController camera;
        camera.attach(window);

        const GLuint objectShader = createShaderProgramFromFiles(
            "shaders/object_vertex.glsl",
            "shaders/object_fragment.glsl"
        );
        const GLuint lightShader = createShaderProgramFromFiles(
            "shaders/vertex.glsl",
            "shaders/fragment.glsl"
        );

        const MeshData tubeMeshData = generateTube(
            app::kTubeInnerRadius,
            app::kTubeOuterRadius,
            app::kTubeHeight,
            app::kTubeSegments
        );
        const MeshData sphereMeshData = generateSphere(
            app::kLightSphereRadius,
            app::kLightSphereSectors,
            app::kLightSphereStacks
        );

        MeshBuffers tubeMesh = createMeshBuffers(tubeMeshData.vertices, tubeMeshData.indices, true);
        MeshBuffers groundMesh = createMeshBuffers(
            app::kGroundVertices.data(),
            app::kGroundVertices.size() * sizeof(float),
            app::kGroundIndices.data(),
            app::kGroundIndices.size() * sizeof(unsigned int),
            true
        );
        MeshBuffers lightMesh = createMeshBuffers(sphereMeshData.vertices, sphereMeshData.indices, false);

        const GLuint tubeTexture = createTexture("wall.jpg");
        const GLuint groundTexture = createTexture("ground.jpg");

        while (!glfwWindowShouldClose(window)) {
            const float currentFrame = static_cast<float>(glfwGetTime());
            camera.updateDeltaTime(currentFrame);
            camera.processInput(window);

            const glm::vec3 lightPos = orbitingLightPosition(currentFrame);
            const glm::mat4 view = camera.viewMatrix();
            const glm::mat4 projection = glm::perspective(
                glm::radians(camera.zoomDegrees()),
                camera.aspectRatio(),
                0.1f,
                100.0f
            );

            glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            drawTexturedMesh(
                objectShader,
                tubeMesh,
                tubeTexture,
                glm::mat4(1.0f),
                view,
                projection,
                lightPos,
                camera.position()
            );
            drawTexturedMesh(
                objectShader,
                groundMesh,
                groundTexture,
                glm::mat4(1.0f),
                view,
                projection,
                lightPos,
                camera.position()
            );
            drawLightMesh(lightShader, lightMesh, lightPos, view, projection);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        destroyMeshBuffers(tubeMesh);
        destroyMeshBuffers(groundMesh);
        destroyMeshBuffers(lightMesh);
        glDeleteTextures(1, &tubeTexture);
        glDeleteTextures(1, &groundTexture);
        glDeleteProgram(objectShader);
        glDeleteProgram(lightShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        if (window != nullptr) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
        return -1;
    }
}
