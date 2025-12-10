#include "App.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

// ===============================
// CALLBACK RATÓN (FPS REAL)
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

    if (app->pitch > 89.0f)  app->pitch = 89.0f;
    if (app->pitch < -89.0f) app->pitch = -89.0f;

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
    cameraPos = glm::vec3(10.0f, 10.0f, 10.0f);
    cameraFront = glm::vec3(-1.0f, -1.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    yaw = -135.0f;
    pitch = -30.0f;

    lastX = 400.0f;
    lastY = 300.0f;
    firstMouse = true;

    sensitivity = 0.1f;
    cameraSpeed = 10.0f;

    init();
}

App::~App() { cleanup(); }

// ===============================
// INIT
// ===============================
void App::init()
{
    if (!glfwInit()) {
        std::cerr << "Error iniciando GLFW\n";
        std::exit(-1);
    }

    stbi_set_flip_vertically_on_load(true);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Terreno OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error creando ventana\n";
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);

    // Activar ratón FPS
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error iniciando GLAD\n";
        std::exit(-1);
    }

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);

    // SHADERS
    materialBasic.load("../shaders/basic.vs", "../shaders/basic.fs");
    materialTerrain.load("../shaders/terrain.vs", "../shaders/terrain.fs");

    // TEXTURAS
    textureStone.load("../textures/TexturePiedra.jpg");
    textureTrunk.load("../textures/TextureTronco.jpg");

    // TERRENO
    if (!heightmap.Load("../textures/heightmap.png", 125.0f)) {
        std::cerr << "ERROR: No se pudo cargar el heightmap\n";
    }

    terrain.BuildFromHeightmap(heightmap);
}

// ===============================
// INPUT TECLADO (WASD)
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
    // ====== TOGGLE WIREFRAME ======
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

}

// ===============================
// RUN
// ===============================
void App::run() { mainLoop(); }

// ===============================
// MAIN LOOP
// ===============================
void App::mainLoop()
{
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ===== TERRENO 3D =====
        if (renderTerrain)
        {
            materialTerrain.use();

            materialTerrain.setVec3("lightDir", glm::vec3(-0.3f, -1.0f, -0.2f));
            materialTerrain.setVec3("viewPos", cameraPos);

            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::lookAt(
                cameraPos,
                cameraPos + cameraFront,
                cameraUp
            );

            glm::mat4 projection = glm::perspective(
                glm::radians(60.0f),
                800.0f / 600.0f,
                0.1f,
                1000.0f
            );

            materialTerrain.setMat4("model", model);
            materialTerrain.setMat4("view", view);
            materialTerrain.setMat4("projection", projection);

           
            terrain.Draw();
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
    geomSquare.cleanup();
    geomTriangle.cleanup();
    geomCircle.cleanup();
    geomSmallSquare.cleanup();

    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}
