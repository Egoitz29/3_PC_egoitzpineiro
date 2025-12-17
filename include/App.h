#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Geometry.h"
#include "Material.h"
#include "Texture.h"
#include "Terrain.h"
#include "HeightmapLoader.h"
#include "TinyModel.h"



// ===============================
// ESTRUCTURA ÁRBOLES
// ===============================
struct TreeInstance
{
    glm::vec3 position;
    float rotationY;
    float scale;
};

class App {
public:
    App();
    ~App();

    void run();

    // ==== Usado por el callback del ratón ====
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    bool firstMouse;
    float sensitivity;

    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

private:
    GLFWwindow* window = nullptr;

    // ==== RENDER ====
    Material materialBasic;
    Material materialTerrain;
    Material materialWater;
    Material materialTree;
    unsigned int grassTexture;
    unsigned int rockTexture;


    Texture textureStone;
    Texture textureTrunk;

    Geometry geomSquare;
    Geometry geomTriangle;
    Geometry geomCircle;
    Geometry geomSmallSquare;
    Geometry geomWater;
    Geometry geomTrunk;
    Geometry geomLeaves;


    int circleVertexCount = 0;

    // ==== TERRENO ====
    Terrain terrain;
    Heightmap heightmap;
    bool renderTerrain = true;

    // ==== ÁRBOLES ====
    std::vector<TreeInstance> trees;

    // ==== CÁMARA ====
    float cameraSpeed;

    TinyModel* treeModel = nullptr;


private:
    void init();
    void processInput(float deltaTime);
    void mainLoop();
    void cleanup();
};
