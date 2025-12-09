#include "Terrain.h"
#include <vector>

bool Terrain::BuildFromHeightmap(const Heightmap& hm)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    int w = hm.width;
    int h = hm.height;

    for (int z = 0; z < h; z++) {
        for (int x = 0; x < w; x++) {

            float y = hm.heights[z * w + x] * 4.0f;


            // POSICIÓN (x, y, z)
            vertices.push_back(x * 0.01f);   // ESCALA X
            vertices.push_back(y * 0.02f);   // ALTURA
            vertices.push_back(z * 0.01f);   // ESCALA Z


            // COLOR (provisional)
            vertices.push_back(0.6f);
            vertices.push_back(0.6f);
            vertices.push_back(0.6f);

            // TEXCOORD (aunque aún no se usen)
            vertices.push_back(x / (float)w);
            vertices.push_back(z / (float)h);
        }
    }

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

    geom.initWithIndices(
        vertices.data(),
        vertices.size() * sizeof(float),
        indices.data(),
        indices.size(),
        8 * sizeof(float),
        (void*)0,                           // posición
        (void*)(3 * sizeof(float)),         // color
        (void*)(6 * sizeof(float))          // UV
    );

    return true;
}

void Terrain::Draw()
{
    geom.draw();
}
