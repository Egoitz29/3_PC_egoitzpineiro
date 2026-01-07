#include "App.h"

#include <iostream>
#include <vector>
#include <cmath>

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
        glTexImage2D(GL_TEXTURE_2D, 0, format,
            width, height, 0, format,
            GL_UNSIGNED_BYTE, data);

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
    cameraSpeed = 30.0f;

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

    window = glfwCreateWindow(800, 600, "Escena OpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, mouse_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ===== SHADERS =====
    materialTerrain.load("shaders/terrain.vs", "shaders/terrain.fs");
    materialWater.load("shaders/water.vs", "shaders/water.fs");
    materialTree.load("shaders/tree.vs", "shaders/tree.fs");
    materialAirplane.load("shaders/airplane.vs", "shaders/airplane.fs");

    // ===== TEXTURAS TERRENO =====
    grassTexture = LoadTexture("textures/Texturagrass.jpg");
    rockTexture = LoadTexture("textures/Texturaroca.jpg");
    waterTexture = LoadTexture("textures/TexturaAgua.jpg"); // 🔹 AGUA
    treeTexture = LoadTexture("models/tree/DB2X2_L01.png");



    // ===== TERRENO =====
    heightmap.Load("textures/heightmap.png", 125.0f);
    terrain.BuildFromHeightmap(heightmap);

    // ===== MODELOS =====
    treeModel = new TinyModel("models/tree/Tree.obj");
    airplaneModel = new TinyModel("models/airplane/11805_airplane_v2_L2.obj");
    airplaneTexture = LoadTexture(
        "models/airplane/airplane_body_diffuse_v1.jpg"
    );



    // ===== ÁRBOLES EN ANILLO =====
    trees.clear();
    glm::vec3 islandCenter(0.0f, 0.0f, 0.0f);
    glm::vec3 dayColor(0.82f, 0.70f, 0.58f);
    glm::vec3 nightColor(0.05f, 0.07f, 0.15f);

    const int NUM_TREES = 24;
    const float RADIUS = 45.0f;

    for (int i = 0; i < NUM_TREES; ++i)
    {
        float angle = (glm::two_pi<float>() / NUM_TREES) * i;

        TreeInstance t;
        float offsetX = 50.0f;
        float offsetZ = 50.0f;

        t.position = glm::vec3(
            islandCenter.x + cos(angle) * RADIUS + offsetX,
            0.0f,
            islandCenter.z + sin(angle) * RADIUS + offsetZ
        );

        t.rotationY = glm::degrees(-angle) + 90.0f;
        t.scale = 5.0f;

        trees.push_back(t);
    }
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

    // AVIÓN
    airplanePos = glm::vec3(5.0f, 18.0f, 5.0f); // centro isla + altura
    airplaneYaw = 45.0f;
    airplaneScale = 0.01f;
    airplaneYaw = 0.0f;   // dirección
    airplanePitch = -8.0f;  // leve ascenso
    airplaneRoll = 0.0f;    // estable
    airplanePrevPos = airplanePos;



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
        float time = (float)glfwGetTime();

        // Velocidad del ciclo (más alto = más lento)
        float cycleSpeed = 0.05f;

        // Valor oscilante entre 0 y 1
        float t = (sin(time * cycleSpeed) + 1.0f) * 0.5f;

        // Interpolación de color
        glm::vec3 skyColor = glm::mix(nightColor, dayColor, t);

        glClearColor(
            skyColor.r,
            skyColor.g,
            skyColor.b,
            1.0f
        );


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(
            glm::radians(60.0f),
            800.0f / 600.0f,
            0.1f, 1000.0f
        );

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
        // ===== AGUA =====
        materialWater.use();
        materialWater.setMat4("view", view);
        materialWater.setMat4("projection", projection);
        materialWater.setVec3("lightDir", sunDir);
        materialWater.setFloat("uTime", (float)glfwGetTime());

        // textura
        materialWater.setInt("waterTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterTexture);

        // 🔹 ESTA VARIABLE ES LOCAL
        glm::mat4 waterModel =
            glm::scale(
                glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.30f, 0.0f)),
                glm::vec3(100.0f)
            );

        materialWater.setMat4("model", waterModel);
        geomWater.draw();



        // ===== ÁRBOLES =====
        materialTree.use();
        materialTree.setVec3("lightDir", sunDir);
        materialTree.setInt("treeTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, treeTexture);


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

        airplaneAngle -= airplaneAngularSpeed * deltaTime;
        if (airplaneAngle < 0.0f)
            airplaneAngle += 360.0f;

        float rad = glm::radians(airplaneAngle);

        // Nueva posición
        airplanePos.x = airplaneCenter.x + cos(rad) * airplaneRadius;
        airplanePos.z = airplaneCenter.z + sin(rad) * airplaneRadius;
        airplanePos.y = 12.0f;

        // Dirección hacia el centro
        glm::vec3 toCenter = airplaneCenter - airplanePos;
        toCenter.y = 0.0f;
        toCenter = glm::normalize(toCenter);

        // Yaw mirando AL CENTRO
        float lookAtCenterYaw = glm::degrees(atan2(toCenter.x, toCenter.z));

        // Offset lateral (90 grados = de lado)
        airplaneYaw = lookAtCenterYaw + 0.0f;


        // ===== AVIÓN =====
        materialAirplane.use();
        materialAirplane.setMat4("view", view);
        materialAirplane.setMat4("projection", projection);
        materialAirplane.setVec3("lightDir", sunDir);

        materialAirplane.setInt("airplaneTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, airplaneTexture);



        // Matriz modelo
        glm::mat4 modelPlane = glm::mat4(1.0f);

        // 1️⃣ Posición
        modelPlane = glm::translate(modelPlane, airplanePos);

        glm::vec3 velocity = airplanePos - airplanePrevPos;
        velocity.y = 0.0f;

        if (glm::length(velocity) > 0.0001f)
        {
            velocity = glm::normalize(velocity);

            // Yaw mirando a la dirección de avance
            airplaneYaw = glm::degrees(atan2(velocity.x, velocity.z));
        }


        // 2️⃣ Corrección FIJA del OBJ (NO se toca más)
        modelPlane = glm::rotate(
            modelPlane,
            glm::radians(-90.0f),
            glm::vec3(1, 0, 0)
        );

        // 3️⃣ UNA SOLA ROTACIÓN de orientación (clave)
        modelPlane = glm::rotate(
            modelPlane,
            glm::radians(airplaneYaw),
            glm::vec3(0, 1, 0)
        );

        // 4️⃣ Escala al final
        modelPlane = glm::scale(modelPlane, glm::vec3(airplaneScale));

        materialAirplane.setMat4("model", modelPlane);
        airplaneModel->Draw();


        //  ESTO FALTABA
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void App::cleanup()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void App::run()
{
    mainLoop();
}
