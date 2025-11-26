#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
    Texture();
    ~Texture();

    bool load(const std::string& path);
    void bind(int unit) const;
    void unbind() const;

private:
    GLuint id;
};
