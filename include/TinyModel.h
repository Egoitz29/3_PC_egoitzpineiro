#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>

class TinyModel
{
public:
    TinyModel(const std::string& path);
    ~TinyModel();

    void Draw() const;

private:
    unsigned int vao = 0;
    unsigned int vbo = 0;
    size_t vertexCount = 0;

    void loadOBJ(const std::string& path);
};
