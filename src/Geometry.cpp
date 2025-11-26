#include "Geometry.h"
#include <glad/glad.h>

Geometry::Geometry()
    : vao(0), vbo(0), ebo(0),
    count(0), useIndices(false),
    mode(GL_TRIANGLES)
{
}

Geometry::~Geometry() {
    cleanup();
}

void Geometry::initWithIndices(const float* vertexData,
    unsigned int vertexDataSize,
    const unsigned int* indexData,
    unsigned int indexCount,
    GLsizei stride,
    const void* posOffset,
    const void* colorOffset,
    const void* uvOffset)
{
    useIndices = true;
    mode = GL_TRIANGLES;
    count = indexCount;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indexData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, posOffset);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, colorOffset);
    glEnableVertexAttribArray(1);

    if (uvOffset) {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, uvOffset);
        glEnableVertexAttribArray(2);
    }

    glBindVertexArray(0);
}

void Geometry::initWithoutIndices(const float* vertexData,
    unsigned int vertexDataSize,
    unsigned int vertexCount,
    GLsizei stride,
    const void* posOffset,
    const void* colorOffset,
    const void* uvOffset,
    GLenum drawMode)
{
    useIndices = false;
    mode = drawMode;
    count = vertexCount;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, posOffset);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, colorOffset);
    glEnableVertexAttribArray(1);

    if (uvOffset) {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, uvOffset);
        glEnableVertexAttribArray(2);
    }

    glBindVertexArray(0);
}

void Geometry::draw() const {
    glBindVertexArray(vao);

    if (useIndices) {
        glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(mode, 0, count);
    }
}

void Geometry::cleanup() {
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);

    vao = vbo = ebo = 0;
}
