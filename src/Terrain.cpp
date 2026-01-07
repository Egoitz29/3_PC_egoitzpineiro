#include "Terrain.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

bool Terrain::BuildFromHeightmap(const Heightmap& hm)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;

    int w = hm.width;
    int h = hm.height;

    // =========================
    // 1. POSICIONES
    // =========================
    for (int z = 0; z < h; z++) {
        for (int x = 0; x < w; x++) {

            float y = hm.heights[z * w + x] * 3.0f;

            positions.push_back(glm::vec3(
                x * 0.10f,
                y * 0.15f,
                z * 0.10f
            ));

            normals.push_back(glm::vec3(0, 1, 0));
        }
    }

    // =========================
    // 2. ÍNDICES
    // =========================
    for (int z = 0; z < h - 1; z++) {
        for (int x = 0; x < w - 1; x++) {

            int tl = z * w + x;
            int tr = tl + 1;
            int bl = (z + 1) * w + x;
            int br = bl + 1;

            indices.push_back(tl);
            indices.push_back(bl);
            indices.push_back(tr);

            indices.push_back(tr);
            indices.push_back(bl);
            indices.push_back(br);
        }
    }

    // =========================
    // 3. CÁLCULO DE NORMALES
    // =========================
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        glm::vec3 v0 = positions[i0];
        glm::vec3 v1 = positions[i1];
        glm::vec3 v2 = positions[i2];

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        normals[i0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;
    }

    for (auto& n : normals)
        n = glm::normalize(n);

    // =========================
    // 4. VERTEX BUFFER (8 floats)
    // =========================
    for (int i = 0; i < positions.size(); i++)
    {
        // POS (3)
        vertices.push_back(positions[i].x);
        vertices.push_back(positions[i].y);
        vertices.push_back(positions[i].z);

        // NORMAL (3)
        vertices.push_back(normals[i].x);
        vertices.push_back(normals[i].y);
        vertices.push_back(normals[i].z);

        // UV (2)
        vertices.push_back((i % w) / (float)w);
        vertices.push_back((i / w) / (float)h);
    }

    // =========================
    // 5. SUBIR A GPU
    // =========================
    geom.initWithIndices(
        vertices.data(),
        vertices.size() * sizeof(float),
        indices.data(),
        indices.size(),
        8 * sizeof(float),              // stride = 8 floats
        (void*)0,                        // pos
        (void*)(3 * sizeof(float)),      // normal
        (void*)(6 * sizeof(float))       // uv
    );

    return true;
}

void Terrain::Draw()
{
    geom.draw();
}
