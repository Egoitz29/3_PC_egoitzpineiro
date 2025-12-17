#include "App.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

// ===============================
// FUNCIÓN CARGA TEXTURAS
// ===============================
unsigned int LoadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
            format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "ERROR cargando textura: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// ===============================
// CALLBACK RATÓN
// ===============================
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (!app) return;

    if (app->firstMouse)
    {
        app->lastX = (float)xpos;
        app->lastY = (float)ypos;
        app->firstMouse = false;
    }

    float xoffset = (float)xpos - app->lastX;
    float yoffset = app->lastY - (float)ypos;

    app->lastX = (float)xpos;
    app->lastY = (float)ypos;

    xoffset *= app->sensitivity;
    yoffset *= app->sensitivity;

    app->yaw += xoffset;
    app->pitch += yoffset;

    app->pitch = glm::clamp(app->pitch, -89.0f, 89.0f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(app->yaw)) * cos(glm::radians(app->pitch));
    direction.y = sin(glm::radians(app->pitch));
    direction.z = sin(glm::radians(app->yaw)) * cos(glm::radians(app->pitch));

    app->cameraFront = glm::normalize(direction);
}

// ===============================
// CONSTRUCTOR / DESTRUCTOR
// ===============================
App::App()
{
    cameraPos = glm::vec3(0.0f, 15.0f, 40.0f);
    cameraFront = glm::normalize(glm::vec3(0.0f, -0.3f, -1.0f));
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    yaw = -90.0f;
    pitch = -15.0f;

    lastX = 400.0f;
    lastY = 300.0f;
    firstMouse = true;

    sensitivity = 0.1f;
    cameraSpeed = 15.0f;

    init();
}

App::~App()
{
    cleanup();
}

// ===============================
// INIT
// ===============================
void App::init()
{
    glfwInit();

    stbi_set_flip_vertically_on_load(true);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Terreno OpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, mouse_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);

    // BLENDING PARA AGUA
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // SHADERS
    materialBasic.load("shaders/basic.vs", "shaders/basic.fs");
    materialTerrain.load("shaders/terrain.vs", "shaders/terrain.fs");
    materialWater.load("shaders/water.vs", "shaders/water.fs");
    materialTree.load("shaders/tree.vs", "shaders/tree.fs");

    // TEXTURAS TERRENO
    grassTexture = LoadTexture("textures/Texturagrass.jpg");
    rockTexture = LoadTexture("textures/Texturaroca.jpg");

    // TERRENO
    heightmap.Load("textures/heightmap.png", 125.0f);
    terrain.BuildFromHeightmap(heightmap);

    // MODELO ÁRBOL
    treeModel = new TinyModel("models/tree/Tree.obj");
    trees.clear();

    glm::vec3 islandCenter = glm::vec3(5.0f, 0.0f, 5.0f);
    const int NUM_TREES = 24;
    const float RADIUS = 5.5f;

    for (int i = 0; i < NUM_TREES; ++i)
    {
        float angle = (glm::two_pi<float>() / NUM_TREES) * i;

        TreeInstance t;

        float x = islandCenter.x + cos(angle) * RADIUS;
        float z = islandCenter.z + sin(angle) * RADIUS;

        // ALTURA TEMPORAL (para que se vean seguro)
        float y = 0.0f;

        t.position = glm::vec3(x, y, z);
        t.rotationY = glm::degrees(-angle) + 90.0f;
        t.scale = 0.6f;

        trees.push_back(t);
    }



    // AGUA (GEOMETRÍA)
    float waterVertices[] = {
        -1,0,-1,  0,0,1,  0,0,
         1,0,-1,  0,0,1,  1,0,
         1,0, 1,  0,0,1,  1,1,
        -1,0, 1,  0,0,1,  0,1
    };

    unsigned int waterIndices[] = { 0,1,2, 2,3,0 };

    geomWater.initWithIndices(
        waterVertices, sizeof(waterVertices),
        waterIndices, 6,
        8 * sizeof(float),
        (void*)0,
        (void*)(3 * sizeof(float)),
        (void*)(6 * sizeof(float))
    );
}

// ===============================
// INPUT
// ===============================
void App::processInput(float deltaTime)
{
    float velocity = cameraSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += velocity * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= velocity * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
}

// ===============================
// MAIN LOOP
// ===============================
void App::mainLoop()
{
    float lastFrame = 0.0f;

    glm::vec3 sunDir = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(deltaTime);

        glClearColor(0.82f, 0.70f, 0.58f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(
            glm::radians(60.0f), 800.0f / 600.0f, 0.1f, 1000.0f);

        // ===== TERRENO =====
        materialTerrain.use();
        materialTerrain.setMat4("model", glm::mat4(1.0f));
        materialTerrain.setMat4("view", view);
        materialTerrain.setMat4("projection", projection);
        materialTerrain.setVec3("lightDir", sunDir);
        materialTerrain.setVec3("viewPos", cameraPos);

        materialTerrain.setInt("texGrass", 0);
        materialTerrain.setInt("texRock", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, rockTexture);

        terrain.Draw();

        // ===== AGUA (TRANSPARENTE, AL FINAL) =====
        materialWater.use();
        materialWater.setFloat("uTime", (float)glfwGetTime());
        materialWater.setMat4("view", view);
        materialWater.setMat4("projection", projection);
        materialWater.setVec3("lightDir", sunDir);
        materialWater.setVec3("viewPos", cameraPos);

        glm::mat4 waterModel =
            glm::scale(
                glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.15f, 0.0f)),
                glm::vec3(50.0f, 1.0f, 50.0f)
            );
        materialWater.setMat4("model", waterModel);
        geomWater.draw();

        // ===== ÁRBOLES =====
        materialTree.use();
        materialTree.setVec3("lightDir", sunDir);
        materialTree.setVec3("lightColor", glm::vec3(1.0f));
        materialTree.setVec3("viewPos", cameraPos);

        for (const TreeInstance& t : trees)
        {
            glm::mat4 modelTree = glm::mat4(1.0f);
            modelTree = glm::translate(modelTree, t.position);
            modelTree = glm::rotate(modelTree, glm::radians(t.rotationY), glm::vec3(0, 1, 0));
            modelTree = glm::scale(modelTree, glm::vec3(t.scale));

            materialTree.setMat4("model", modelTree);
            materialTree.setMat4("view", view);
            materialTree.setMat4("projection", projection);

            treeModel->Draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

// ===============================
// CLEANUP
// ===============================
void App::cleanup()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void App::run()
{
    mainLoop();
}
