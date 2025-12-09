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
