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


    TinyModel* airplaneModel;

    glm::vec3 airplanePos;
    float airplaneYaw;
    float airplaneScale;
    float airplanePitch;
    float airplaneRoll;
    glm::vec3 airplaneCenter = glm::vec3(5.0f, 0.0f, 5.0f); // centro de la rotonda
    glm::vec3 airplanePrevPos;
    glm::vec3 dayColor;
    glm::vec3 nightColor;

    float airplaneRadius = 6.0f;      // radio del giro
    float airplaneAngle = 45.0f;       // ángulo actual
    float airplaneAngularSpeed = 45.0f; // grados por segundo
    // radio (igual que árboles)


private:
    GLFWwindow* window = nullptr;

    // ==== RENDER ====
    Material materialBasic;
    Material materialTerrain;
    Material materialWater;
    Material materialTree;
    Material materialAirplane;

    unsigned int treeTexture;
    unsigned int grassTexture;
    unsigned int rockTexture;
    unsigned int waterTexture;
    unsigned int airplaneTexture;
 
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
    glm::vec3 islandCenter;

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
