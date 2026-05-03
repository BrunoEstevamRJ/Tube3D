#pragma once

#include <glad/gl.h>

#include <string>

class Texture {
public:
    Texture() = default;
    explicit Texture(const std::string& imagePath);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void load(const std::string& imagePath);
    void bind(GLenum textureUnit) const;

private:
    GLuint texture_ = 0;
};
