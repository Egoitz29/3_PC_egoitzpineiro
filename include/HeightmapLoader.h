#pragma once
#include <vector>
#include <string>

class Heightmap {
public:
    int width, height;
    std::vector<float> heights;

    bool Load(const std::string& path, float maxHeight = 20.0f);


    float GetHeightAt(float worldX, float worldZ) const;

};
