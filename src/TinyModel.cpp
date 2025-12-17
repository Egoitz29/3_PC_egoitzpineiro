#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "TinyModel.h"
#include <iostream>
#include <vector>
#include <cfloat>
#include <glm/glm.hpp>

// ===============================
// CONSTRUCTOR / DESTRUCTOR
// ===============================
TinyModel::TinyModel(const std::string& path)
{
    loadOBJ(path);
}

TinyModel::~TinyModel()
{
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

// ===============================
// CARGA OBJ
// ===============================
void TinyModel::loadOBJ(const std::string& path)
{
    std::cout << "Cargando OBJ desde: " << path << std::endl;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Carpeta base del modelo (para .mtl y texturas)
    std::string baseDir = path.substr(0, path.find_last_of("/\\") + 1);

    bool ret = tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &warn,
        &err,
        path.c_str(),
        baseDir.c_str(),
        true
    );

    if (!warn.empty()) std::cout << warn << std::endl;
    if (!err.empty())  std::cerr << err << std::endl;
    if (!ret) return;

    std::vector<float> vertices;

    // ===============================
    // 1. Cargar vértices (POS + NORMAL + UV)
    // Formato: [Px Py Pz | Nx Ny Nz | U V] = 8 floats
    // ===============================
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            // POSICIÓN
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            // NORMAL (si viene, la metemos; luego recalculamos igual)
            if (index.normal_index >= 0 && !attrib.normals.empty())
            {
                vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
                vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
                vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
            }
            else
            {
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);
            }

            // TEXCOORD
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

    // ===============================
    // 2. Calcular límites del modelo
    // ===============================
    glm::vec3 minV(FLT_MAX);
    glm::vec3 maxV(-FLT_MAX);

    for (size_t i = 0; i < vertices.size(); i += 8)
    {
        glm::vec3 v(vertices[i], vertices[i + 1], vertices[i + 2]);
        minV = glm::min(minV, v);
        maxV = glm::max(maxV, v);
    }

    // Centro en XZ, base en Y = 0
    float centerX = (minV.x + maxV.x) * 0.5f;
    float centerZ = (minV.z + maxV.z) * 0.5f;
    float minY = minV.y;

    // ===============================
    // 3. Recolocar vértices
    // ===============================
    for (size_t i = 0; i < vertices.size(); i += 8)
    {
        vertices[i + 0] -= centerX; // X centrado
        vertices[i + 1] -= minY;    // base en Y = 0
        vertices[i + 2] -= centerZ; // Z centrado
    }

    // ===============================
    // 4. Recalcular NORMALES (FLAT) por triángulo
    // Como dibujas con glDrawArrays, cada 3 vértices = 1 triángulo.
    // ===============================
    size_t vcount = vertices.size() / 8;

    for (size_t i = 0; i + 2 < vcount; i += 3)
    {
        glm::vec3 v0(
            vertices[(i + 0) * 8 + 0],
            vertices[(i + 0) * 8 + 1],
            vertices[(i + 0) * 8 + 2]
        );
        glm::vec3 v1(
            vertices[(i + 1) * 8 + 0],
            vertices[(i + 1) * 8 + 1],
            vertices[(i + 1) * 8 + 2]
        );
        glm::vec3 v2(
            vertices[(i + 2) * 8 + 0],
            vertices[(i + 2) * 8 + 1],
            vertices[(i + 2) * 8 + 2]
        );

        glm::vec3 n = glm::cross(v1 - v0, v2 - v0);
        float len = glm::length(n);
        if (len > 0.000001f) n /= len;
        else n = glm::vec3(0.0f, 1.0f, 0.0f);

        // asignar a los 3 vértices del triángulo
        for (int j = 0; j < 3; ++j)
        {
            vertices[(i + j) * 8 + 3] = n.x;
            vertices[(i + j) * 8 + 4] = n.y;
            vertices[(i + j) * 8 + 5] = n.z;
        }
    }

    // ===============================
    // 5. Subir a GPU
    // ===============================
    vertexCount = vcount;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    GLsizei stride = 8 * sizeof(float);

    // POSICIÓN
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // NORMAL
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TEXCOORD
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// ===============================
// DRAW
// ===============================
void TinyModel::Draw() const
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertexCount);
}
