#include "ecsgame.h"


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../config.h"
#include "../geometry/meshfactory.h"
#include <algorithm>
#include <memory>
#include <iostream>



#include "../utils/stats.h"
#include "../utils/random.h"

bool Ecs::Game::init() {
    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window.reset(glfwCreateWindow(Config::Camera::SCREEN_WIDTH, Config::Camera::SCREEN_HEIGHT, "Arkanoid ECS", nullptr, nullptr));
    if (!window) return false;
    glfwMakeContextCurrent(window.get());
    glfwSwapInterval(0);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;


    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Renderer:       " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Vendor:         " << glGetString(GL_VENDOR) << std::endl;

    glEnable(GL_DEPTH_TEST);

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "OpenGL numeric version: " << major << "." << minor << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    ImGui_ImplOpenGL3_Init("#version 450");
    view = glm::lookAt(Config::Camera::CAMERA_POS, Config::Camera::CAMERA_POS + Config::Camera::CAMERA_FRONT, Config::Camera::CAMERA_UP);
    proj = glm::perspective(glm::radians(45.0f), (float)Config::Camera::SCREEN_WIDTH / Config::Camera::SCREEN_HEIGHT, 0.1f, 100.0f);
    initResources();
    resetGame();
    return true;
}

void Ecs::Game::run() {

    float lastTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window.get())) {
        float now = (float)glfwGetTime();
        float dt = std::min(now - lastTime, 0.05f);
        lastTime = now;

        glfwPollEvents();
        if (glfwGetKey(window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window.get(), true);
        if (manager.globalState.gameOver && glfwGetKey(window.get(), GLFW_KEY_R) == GLFW_PRESS) resetGame();

        inputSystem.Update(manager, window.get(), dt);

        if (!manager.globalState.gameOver) {
            physicsSystem.Update(manager, dt);
            powerUpSystem.Update(manager, dt);
            logicSystem.Update(manager);
        }

        uboCamera->bind();
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
        uboCamera->unbind();

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderSystem.Update(manager, *shader);
        stats.update(dt);
        renderSystem.DrawUI(manager, stats,
            //todo fix put pointers to functin as render system members and move to initialize
            [&]() {this->resetGame();},
            [&]() {glfwSetWindowShouldClose(window.get(), true);}
            );

        glfwSwapBuffers(window.get());
    }
    cleanup();
}

void Ecs::Game::initResources() {

    shader = std::make_unique<Shader>("shaders/arkanoid.vert", "shaders/arkanoid.frag");
    cubeMesh = MeshFactory::createCube();
    sphereMesh = MeshFactory::createSphere(Config::Ball::RADIUS);

    uboCamera = std::make_unique<Buffer>(GL_UNIFORM_BUFFER);
    uboCamera->allocate(2 * sizeof(glm::mat4));
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboCamera->ID);

    renderSystem.Init();
    renderSystem.SetupVAO(*cubeMesh->vao);
    renderSystem.SetupVAO(*sphereMesh->vao);

    //mesh for powerup do PhysicsSystem (aby mohl spawnovat)
    physicsSystem.powerUpMesh = cubeMesh.get();
}

void Ecs::Game::resetGame() {
    manager.clear();
    manager.globalState = {0, Config::Stats::INITIAL_LIVES, false, false};

    Entity paddle = manager.createEntity();
    manager.addEntityComponent(paddle, TagComponent{TagType::Paddle});
    manager.addEntityComponent(paddle, TransformComponent{Config::Paddle::START_POS, Config::Paddle::SCALE});
    manager.addEntityComponent(paddle, RenderComponent{cubeMesh.get(), glm::vec4(0.3f, 0.8f, 0.3f, 1.0f)});
    manager.addEntityComponent(paddle, ColliderComponent{ColliderComponent::Box});
    manager.addEntityComponent(paddle, PlayerControlComponent{});

    Entity ball = manager.createEntity();
    manager.addEntityComponent(ball, TagComponent{TagType::Ball});
    manager.addEntityComponent(ball, TransformComponent{Config::Ball::START_POS, glm::vec3(Config::Ball::RADIUS)});
    manager.addEntityComponent(ball, RigidbodyComponent{Config::Ball::START_VEL});
    manager.addEntityComponent(ball, RenderComponent{sphereMesh.get(), glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)});
    manager.addEntityComponent(ball, ColliderComponent{ColliderComponent::Sphere, Config::Ball::RADIUS});
    manager.addEntityComponent(ball, GameStateComponent{false});

    int rows = Config::Bricks::ROWS;
    int cols = Config::Bricks::COLS;
    float totalWidth = Config::World::MAX_X - Config::World::MIN_X;
    float spacingX = 0.2f;
    float brickWidth = (totalWidth - (cols - 1) * spacingX) / cols;
    float brickHeight = Config::Bricks::SCALE.y;
    float startX = Config::World::MIN_X + brickWidth * 0.5f;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            Entity brick = manager.createEntity();
            glm::vec3 pos = {
                startX + c * (brickWidth + spacingX),
                Config::Bricks::START_Y + r * (brickHeight + spacingX),
                0.0f
            };
            glm::vec3 scale = {brickWidth, brickHeight, Config::Bricks::SCALE.z};
            manager.addEntityComponent(brick, TagComponent{TagType::Brick});
            manager.addEntityComponent(brick, TransformComponent{pos, scale});
            manager.addEntityComponent(brick, RenderComponent{cubeMesh.get(), Random::RandomColor()});
            manager.addEntityComponent(brick, ColliderComponent{ColliderComponent::Box});
        }
    }
}

void Ecs::Game::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
