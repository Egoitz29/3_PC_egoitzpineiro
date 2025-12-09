#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

// ✅ GLM OBLIGATORIO AQUÍ (ESTABA FALTANDO)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

private:
    GLFWwindow* window = nullptr;

    // =========================
    // SISTEMA DE RENDER
    // =========================
    Material materialBasic;
    Material materialTerrain;

    Texture textureStone;
    Texture textureTrunk;

    Geometry geomSquare;
    Geometry geomTriangle;
    Geometry geomCircle;
    Geometry geomSmallSquare;

    int circleVertexCount = 0;

    // =========================
    // TERRENO
    // =========================
    Terrain terrain;
    Heightmap heightmap;
    bool renderTerrain = true;

    // =========================
    // CÁMARA 3D
    // =========================
    glm::vec3 cameraPos = glm::vec3(30.0f, 20.0f, 30.0f);
    glm::vec3 cameraFront = glm::vec3(-1.0f, -0.5f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = -135.0f;
    float pitch = -25.0f;
    float lastX = 400, lastY = 300;
    bool firstMouse = true;

    float cameraSpeed = 20.0f;
    float sensitivity = 0.1f;

private:
    void init();
    void processInput(float deltaTime);



    void mainLoop();
    void cleanup();
};
