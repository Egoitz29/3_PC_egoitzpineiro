#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "Geometry.h"
#include "Material.h"
#include "Texture.h"

class App {
public:
    App();
    ~App();

    void run();

private:
    GLFWwindow* window = nullptr;

    // SISTEMA DE RENDER
    Material materialBasic;

    Texture textureStone;
    Texture textureTrunk;

    Geometry geomSquare;
    Geometry geomTriangle;
    Geometry geomCircle;
    Geometry geomSmallSquare;

    int circleVertexCount = 0;

private:
    void init();
    void initMainSquare();
    void initTriangle();
    void initCircle();
    void initSmallSquare();

    void mainLoop();
    void cleanup();
};
