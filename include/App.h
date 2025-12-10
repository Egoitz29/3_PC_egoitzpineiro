#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include <glm/glm.hpp>

#include "Geometry.h"
#include "Material.h"
#include "Texture.h"
#include "Terrain.h"
#include "HeightmapLoader.h"

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

    Texture textureStone;
    Texture textureTrunk;

    Geometry geomSquare;
    Geometry geomTriangle;
    Geometry geomCircle;
    Geometry geomSmallSquare;

    int circleVertexCount = 0;

    // ==== TERRENO ====
    Terrain terrain;
    Heightmap heightmap;
    bool renderTerrain = true;

    float cameraSpeed;

private:
    void init();
    void processInput(float deltaTime);

    void mainLoop();
    void cleanup();
};
