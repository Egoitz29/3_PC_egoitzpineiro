#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "TinyModel.h"
#include <iostream>
#include <vector>
#include <cfloat>   // FLT_MAX
#include <glm/glm.hpp>

TinyModel::TinyModel(const std::string& path)
{
    loadOBJ(path);
}

TinyModel::~TinyModel()
{
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void TinyModel::loadOBJ(const std::string& path)
{
    std::cout << "Cargando OBJ desde: " << path << std::endl;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &warn,
        &err,
        path.c_str(),
        "models/tree/",
        true
    );

    if (!warn.empty()) std::cout << warn << std::endl;
    if (!err.empty()) std::cerr << err << std::endl;
    if (!ret) return;

    std::vector<float> vertices;

    // -----------------------------
    // 1. Cargar vértices
    // -----------------------------
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            if (index.texcoord_index >= 0 && !attrib.texcoords.empty())
            {
                vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
            }
            else
            {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
        }
    }

    // -----------------------------
    // 2. Calcular límites del modelo
    // -----------------------------
    glm::vec3 minV(FLT_MAX);
    glm::vec3 maxV(-FLT_MAX);

    for (size_t i = 0; i < vertices.size(); i += 5)
    {
        glm::vec3 v(vertices[i], vertices[i + 1], vertices[i + 2]);
        minV = glm::min(minV, v);
        maxV = glm::max(maxV, v);
    }

    // Centro en XZ, base en Y = 0
    float centerX = (minV.x + maxV.x) * 0.5f;
    float centerZ = (minV.z + maxV.z) * 0.5f;
    float minY = minV.y;

    // -----------------------------
    // 3. Recolocar vértices
    // -----------------------------
    for (size_t i = 0; i < vertices.size(); i += 5)
    {
        vertices[i] -= centerX; // X centrado
        vertices[i + 1] -= minY;    // base en Y = 0
        vertices[i + 2] -= centerZ; // Z centrado
    }

    // -----------------------------
    // 4. Subir a GPU
    // -----------------------------
    vertexCount = vertices.size() / 5;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void TinyModel::Draw() const
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertexCount);
}
