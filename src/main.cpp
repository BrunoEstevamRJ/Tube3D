/*
 * Tube.cpp
 *
 * Renderização de um tubo 3D com OpenGL
 * Autor: BrunoEstevamRJ
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WINDOW_TITLE "Tube Renderer"

float lastX = 0.0f, lastY = 0.0f;
float yaw = 0.0f, pitch = 0.0f;
bool firstMouse = true;
bool isDragging = false;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
            firstMouse = true;
        } else if (action == GLFW_RELEASE) {
            isDragging = false;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!isDragging) return;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; 
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch -= yoffset; 

    
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 LightPos;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    LightPos = lightPos;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 LightPos;

uniform sampler2D texture1;
uniform vec3 viewPos;

out vec4 FragColor;

void main() {
    /*--[ Iluminação básica ]--*/
    vec3 color = texture(texture1, TexCoord).rgb;
    
    /*--[ Ambient ]--*/
    vec3 ambient = 0.3 * color;
    
    /*--[ Diffuse ]--*/
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    /*--[ Specular ]--*/
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = vec3(0.3) * spec;
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
)";

void generateTube(std::vector<float>& vertices, std::vector<unsigned int>& indices, 
                  float innerRadius, float outerRadius, float height, int segments) {
    const float PI = 3.14159265359f;
    vertices.clear();
    indices.clear();
    
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * PI * i / segments;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);
        
        vertices.push_back(outerRadius * cosTheta);
        vertices.push_back(outerRadius * sinTheta);
        vertices.push_back(0.0f);                  
        vertices.push_back(cosTheta);              
        vertices.push_back(sinTheta);              
        vertices.push_back(0.0f);                  
        vertices.push_back(2.0f * (float)i / segments);
        vertices.push_back(0.0f);
        
        vertices.push_back(outerRadius * cosTheta);
        vertices.push_back(outerRadius * sinTheta);
        vertices.push_back(height);
        vertices.push_back(cosTheta);
        vertices.push_back(sinTheta);
        vertices.push_back(0.0f);
        vertices.push_back(2.0f * (float)i / segments); 
        vertices.push_back(1.0f);
        
        vertices.push_back(innerRadius * cosTheta);
        vertices.push_back(innerRadius * sinTheta);
        vertices.push_back(0.0f);
        vertices.push_back(-cosTheta);             
        vertices.push_back(-sinTheta);
        vertices.push_back(0.0f);
        vertices.push_back(2.0f * (float)i / segments);
        vertices.push_back(0.0f);
        
        vertices.push_back(innerRadius * cosTheta);
        vertices.push_back(innerRadius * sinTheta);
        vertices.push_back(height);
        vertices.push_back(-cosTheta);
        vertices.push_back(-sinTheta);
        vertices.push_back(0.0f);
        vertices.push_back(2.0f * (float)i / segments); 
        vertices.push_back(1.0f);              
    }
    
    for (int i = 0; i < segments; i++) {
        int next = (i + 1) % segments;
        int i0 = i * 4, i1 = i * 4 + 1, i2 = i * 4 + 2, i3 = i * 4 + 3;
        int n0 = next * 4, n1 = next * 4 + 1, n2 = next * 4 + 2, n3 = next * 4 + 3;
        
        
        indices.insert(indices.end(), { i0, i1, n1, i0, n1, n0 });
        indices.insert(indices.end(), { i2, n2, n3, i2, n3, i3 });
        indices.insert(indices.end(), { i1, n1, n3, i1, n3, i3 });
        indices.insert(indices.end(), { i0, n0, n2, i0, n2, i2 });
    }
}

GLuint CompileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Erro ao compilar shader:\n" << log << std::endl;
    }

    return shader;
}

GLuint CreateShaderProgram() {
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Erro ao linkar programa:\n" << log << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

GLuint CreateTexture(const char* imagePath) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Erro ao carregar textura: " << imagePath << std::endl;
        
        unsigned char white[] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    }
    
    stbi_image_free(data);
    return texture;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Erro ao inicializar GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, WINDOW_TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "Erro ao criar janela GLFW\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Erro ao carregar OpenGL\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);

    GLuint shaderProgram = CreateShaderProgram();

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateTube(vertices, indices, 1.0f, 0.6f, 2.0f, 32);

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    const GLuint stride = 8 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLuint texture = CreateTexture("wall.jpg");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, 2.0f);
        glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, 5.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewPos));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}