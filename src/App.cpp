#include "App.h"
#include <iostream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "stb_image.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ===============================
// VARIABLES CÁMARA
// ===============================
glm::vec3 cameraPos = glm::vec3(10.0f, 10.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(-1.0f, -1.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float cameraSpeed = 10.0f;
float yaw = -135.0f;
float pitch = -30.0f;
float lastX = 400, lastY = 300;
float sensitivity = 0.1f;
bool firstMouse = true;

// ===============================
// CALLBACK RATÓN
// ===============================
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (!app) return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// ===============================
// CONSTRUCTOR / DESTRUCTOR
// ===============================
App::App() { init(); }
App::~App() { cleanup(); }

// ===============================
// INIT
// ===============================
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
// INPUT TECLADO
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
// RUN
// ===============================
void App::run() { mainLoop(); }

// ===============================
// MAIN LOOP
// ===============================
void App::mainLoop() {

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {

        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ============================
        // MODO TERRENO 3D
        // ============================
        if (renderTerrain) {

            materialTerrain.use();

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

            materialTerrain.setMat4("model", glm::value_ptr(model));
            materialTerrain.setMat4("view", glm::value_ptr(view));
            materialTerrain.setMat4("projection", glm::value_ptr(projection));

            // Si lo quieres sólido:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            terrain.Draw();

            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        // ============================
        // MODO 2D
        // ============================
        materialBasic.use();

        float time = glfwGetTime();
        float angle = time * 0.4f;
        float mixFactor = 0.5f;

        // CUADRADO GRANDE
        materialBasic.setBool("useTexture", true);
        materialBasic.setBool("useDoubleTexture", false);
        materialBasic.setFloat("rotation", angle);

        textureStone.bind(0);
        materialBasic.setInt("tex1", 0);
        geomSquare.draw();

        // TRIÁNGULO + CÍRCULO
        materialBasic.setBool("useTexture", false);
        materialBasic.setBool("useDoubleTexture", false);
        materialBasic.setFloat("rotation", 0.0f);

        geomTriangle.draw();
        geomCircle.draw();

        // CUADRADO PEQUEÑO
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


// ===============================
// CLEANUP
// ===============================
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
