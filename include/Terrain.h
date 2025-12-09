#pragma once
#include "HeightmapLoader.h"
#include "Geometry.h"

class Terrain {
public:
    Geometry geom;
    bool BuildFromHeightmap(const Heightmap& hm);
    void Draw();
};
