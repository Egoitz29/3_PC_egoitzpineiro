#include "HeightmapLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool Heightmap::Load(const std::string& path, float maxHeight)
{
    int channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 1);

    if (!data)
        return false;

    heights.resize(width * height);

    for (int z = 0; z < height; z++) {
        for (int x = 0; x < width; x++) {

            unsigned char pixel = data[z * width + x];

            float h = (pixel / 255.0f) * maxHeight;
            heights[z * width + x] = h;
        }
    }

    stbi_image_free(data);
    return true;
}

float Heightmap::GetHeightAt(float worldX, float worldZ) const
{
    // Convertir coordenadas mundo → coordenadas del heightmap
    float halfW = width * 0.5f;
    float halfH = height * 0.5f;

    int x = static_cast<int>(worldX + halfW);
    int z = static_cast<int>(worldZ + halfH);


    // Clamp para no salir del mapa
    x = std::max(0, std::min(x, width - 1));
    z = std::max(0, std::min(z, height - 1));

    return heights[z * width + x];
}

