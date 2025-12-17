#pragma once

#include <glad/glad.h>

class Geometry {
public:
    Geometry();
    ~Geometry();

    // Para geometrías con índices (cuadrados)
        void initWithIndices(const float* vertexData,
        unsigned int vertexDataSize,
        const unsigned int* indexData,
        unsigned int indexCount,
        GLsizei stride,
        const void* posOffset,
        const void* colorOffset,
        const void* uvOffset);

    // Para geometrías sin índices (triángulo, círculo…)
    void initWithoutIndices(const float* vertexData,
        unsigned int vertexDataSize,
        unsigned int vertexCount,
        GLsizei stride,
        const void* posOffset,
        const void* colorOffset,
        const void* uvOffset,
        GLenum drawMode = GL_TRIANGLES);

    void draw() const;
    void cleanup();

private:
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLsizei count;
    bool    useIndices;
    GLenum  mode;
};
