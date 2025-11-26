#include "App.h"
#include <iostream>
#include <vector>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

App::App() { init(); }
App::~App() { cleanup(); }

void App::init() {

    if (!glfwInit()) {
        std::cerr << "Error iniciando GLFW\n";
        std::exit(-1);
    }

    stbi_set_flip_vertically_on_load(true);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Figuras OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error creando ventana\n";
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error iniciando GLAD\n";
        std::exit(-1);
    }

    glViewport(0, 0, 800, 600);


    // =========================================
    // CARGA DE MATERIAL (SHADER)
    // =========================================
    materialBasic.load("../shaders/basic.vs", "../shaders/basic.fs");


    // =========================================
    // GEOMETRÍAS
    // =========================================
    initMainSquare();
    initTriangle();
    initCircle();
    initSmallSquare();


    // =========================================
    // TEXTURAS
    // =========================================
    textureStone.load("../textures/TexturePiedra.jpg");
    textureTrunk.load("../textures/TextureTronco.jpg");
}


void App::initMainSquare() {

    float vertices[] = {
        0.0f, -0.7f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
        0.7f, -0.7f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        0.7f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
        0.0f,  0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    geomSquare.initWithIndices(
        vertices,
        sizeof(vertices),
        indices,
        6,
        8 * sizeof(float),
        (void*)0,
        (void*)(3 * sizeof(float)),
        (void*)(6 * sizeof(float))
    );
}

void App::initTriangle() {

    float vertices[] = {
        0.45f, 0.75f, 0.0f,  1.0f, 0.0f, 0.0f,
        0.85f, 0.35f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.05f, 0.35f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    geomTriangle.initWithoutIndices(
        vertices,
        sizeof(vertices),
        3,
        6 * sizeof(float),
        (void*)0,
        (void*)(3 * sizeof(float)),
        nullptr
    );
}

void App::initCircle() {

    std::vector<float> vertices;
    float centerX = -0.5f;
    float centerY = 0.5f;
    float radius = 0.25f;
    int segments = 40;

    // centro
    vertices.push_back(centerX);
    vertices.push_back(centerY);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(1.0f);

    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        vertices.push_back(centerX + cos(angle) * radius);
        vertices.push_back(centerY + sin(angle) * radius);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
    }

    circleVertexCount = segments + 2;

    geomCircle.initWithoutIndices(
        vertices.data(),
        vertices.size() * sizeof(float),
        circleVertexCount,
        6 * sizeof(float),
        (void*)0,
        (void*)(3 * sizeof(float)),
        nullptr,
        GL_TRIANGLE_FAN
    );
}

void App::initSmallSquare() {

    float vertices[] = {
        -0.85f, -0.85f, 0.0f,   0.2f, 0.8f, 1.0f,    0.0f, 0.0f,
        -0.35f, -0.85f, 0.0f,   0.2f, 0.8f, 1.0f,    1.0f, 0.0f,
        -0.35f, -0.35f, 0.0f,   0.2f, 0.8f, 1.0f,    1.0f, 1.0f,
        -0.85f, -0.35f, 0.0f,   0.2f, 0.8f, 1.0f,    0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    geomSmallSquare.initWithIndices(
        vertices,
        sizeof(vertices),
        indices,
        6,
        8 * sizeof(float),
        (void*)0,
        (void*)(3 * sizeof(float)),
        (void*)(6 * sizeof(float))
    );
}

void App::run() { mainLoop(); }

void App::mainLoop() {

    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        materialBasic.use();

        float time = glfwGetTime();
        float angle = time * 0.4f;
        float mixFactor = 0.5f;


        // ========= CUADRADO GRANDE =========
        materialBasic.setBool("useTexture", true);
        materialBasic.setBool("useDoubleTexture", false);

        materialBasic.setFloat("rotation", angle);

        textureStone.bind(0);
        materialBasic.setInt("tex1", 0);

        geomSquare.draw();


        // ========= TRIÁNGULO + CÍRCULO =========
        materialBasic.setBool("useTexture", false);
        materialBasic.setBool("useDoubleTexture", false);
        materialBasic.setFloat("rotation", 0.0f);

        geomTriangle.draw();
        geomCircle.draw();


        // ========= CUADRADO PEQUEÑO (mezcla) =========
        materialBasic.setBool("useTexture", true);
        materialBasic.setBool("useDoubleTexture", true);

        materialBasic.setFloat("mixFactor", mixFactor);

        textureStone.bind(0);
        textureTrunk.bind(1);

        materialBasic.setInt("tex1", 0);
        materialBasic.setInt("tex2", 1);

        geomSmallSquare.draw();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void App::cleanup() {

    geomSquare.cleanup();
    geomTriangle.cleanup();
    geomCircle.cleanup();
    geomSmallSquare.cleanup();

    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}
