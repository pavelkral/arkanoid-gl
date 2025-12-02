#include "oopgame.h"

#include <iostream>
#include <algorithm>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../utils/random.h"
#include "../utils/math.h"
#include "../gl/vertexarray.h"
#include "../geometry/meshfactory.h"


bool Game::init() {

    if (!glfwInit()) return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window.reset(glfwCreateWindow(Config::Camera::SCREEN_WIDTH, Config::Camera::SCREEN_HEIGHT, "Arkanoid OOP", nullptr, nullptr));
    if (!window) return false;

    glfwMakeContextCurrent(window.get());
    glfwSwapInterval(0); // VSync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Renderer:       " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Vendor:         " << glGetString(GL_VENDOR) << std::endl;

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    std::cout << "OpenGL numeric version: " << major << "." << minor << std::endl;

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
    ImGui_ImplOpenGL3_Init("#version 450 core");

    view = glm::lookAt(Config::Camera::CAMERA_POS,Config::Camera::CAMERA_POS + Config::Camera::CAMERA_FRONT,Config::Camera::CAMERA_UP);
    proj = glm::perspective(glm::radians(45.0f), (float)Config::Camera::SCREEN_WIDTH / Config::Camera::SCREEN_HEIGHT, 0.1f, 100.0f);
    initResources();
    resetGame();
    return true;
}

void Game::run() {

    float lastTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window.get())) {
        float now = (float)glfwGetTime();
        float dt = std::min(now - lastTime, 0.05f); // Clamp dt
        lastTime = now;

        glfwPollEvents();
        processInput(dt);

        if (!gameOver) {
            updatePhysics(dt);
        }

        render();
        renderUI();

        glfwSwapBuffers(window.get());

        stats.update(dt);
    }
    cleanup();
}

void Game::initResources() {

    shader = std::make_unique<Shader>("shaders/arkanoid.vert", "shaders/arkanoid.frag");
    cubeMesh = MeshFactory::createCube();
    sphereMesh = MeshFactory::createSphere(Config::Ball::RADIUS);

    uboCamera = std::make_unique<Buffer>(GL_UNIFORM_BUFFER);
    uboCamera->allocate(2 * sizeof(glm::mat4));
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboCamera->ID);

    size_t maxInstances = Config::Bricks::ROWS * Config::Bricks::COLS + 10;
    vboInstance = std::make_unique<Buffer>(GL_ARRAY_BUFFER);
    vboInstance->allocate(maxInstances * sizeof(glm::mat4));

    vboColor = std::make_unique<Buffer>(GL_ARRAY_BUFFER);
    vboColor->allocate(maxInstances * sizeof(glm::vec4));

    auto setupAttribs = [&](VertexArray& vao) {
        vao.bind();
        vboInstance->bind();
        size_t vec4Size = sizeof(glm::vec4);
        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(1 + i);
            glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
            glVertexAttribDivisor(1 + i, 1);
        }
        vboColor->bind();
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
        glVertexAttribDivisor(5, 1);
        vao.unbind();
    };

    setupAttribs(*cubeMesh->vao);
    setupAttribs(*sphereMesh->vao);

    renderMatrices.reserve(maxInstances);
    renderColors.reserve(maxInstances);
}

void Game::resetGame() {
    score = 0;
    lives = Config::Stats::INITIAL_LIVES;
    gameOver = false;
    gameWon = false;
    ballLaunched = false;
    bricks.clear();

    int rows = Config::Bricks::ROWS;
    int cols = Config::Bricks::COLS;
    float totalWidth = Config::World::MAX_X - Config::World::MIN_X;
    float spacingX = 0.2f;
    float brickWidth = (totalWidth - (cols - 1) * spacingX) / cols;
    float brickHeight = Config::Bricks::SCALE.y;
    float startX = Config::World::MIN_X + brickWidth * 0.5f;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            glm::vec3 pos = {
                startX + c * (brickWidth + spacingX),
                Config::Bricks::START_Y + r * (brickHeight + spacingX),
                0.0f
            };
            glm::vec3 scale = {brickWidth, brickHeight, Config::Bricks::SCALE.z};
            bricks.emplace_back(pos, scale, Random::RandomColor());
        }
    }

    resetBallPaddle();
}

void Game::resetBallPaddle() {
    paddle.transform.pos = Config::Paddle::START_POS;
    paddle.transform.scale = Config::Paddle::SCALE;

    ballLaunched = false;
    ball.transform.scale = glm::vec3(Config::Ball::RADIUS, Config::Ball::RADIUS, Config::Ball::RADIUS);
    ball.radius = Config::Ball::RADIUS;
    ball.velocity = Config::Ball::START_VEL;

    lastPaddleX = paddle.transform.pos.x;
    paddleVelocityX = 0.0f;
}

void Game::processInput(float dt) {
    if (glfwGetKey(window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window.get(), true);
    // Mouse control
    double xpos, ypos;
    glfwGetCursorPos(window.get(), &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(window.get(), &width, &height);
    // mouse X to World X
    float normX = (float)xpos / width; // 0..1
    float worldX = Config::World::MIN_X + normX * (Config::World::MAX_X - Config::World::MIN_X);

    //GOAL Spin
    float prevPaddleX = paddle.transform.pos.x;
    paddle.transform.pos.x += (worldX - paddle.transform.pos.x) * 15.0f * dt;

    float halfW = paddle.transform.scale.x * 0.5f;
    paddle.transform.pos.x = std::clamp(paddle.transform.pos.x, Config::World::MIN_X + halfW, Config::World::MAX_X - halfW);

    // guard dt > 0
    if (dt > 0.0f) {
        paddleVelocityX = (paddle.transform.pos.x - prevPaddleX) / dt;
    } else {
        paddleVelocityX = 0.0f;
    }
    lastPaddleX = paddle.transform.pos.x;

    //GOAL Space to launch ball
    if (glfwGetKey(window.get(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!ballLaunched && !gameOver && !gameWon) {
            ballLaunched = true;
        }
    }
}

void Game::updatePhysics(float dt) {
    //GOAL Hold ball on paddle
    if (!ballLaunched) {
        ball.transform.pos.x = paddle.transform.pos.x;
        ball.transform.pos.y = paddle.transform.pos.y + paddle.transform.scale.y * 0.5f + ball.radius + 0.2f;
        return;
    }

    ball.transform.pos += ball.velocity * dt;

    // Walls Collision
    if (ball.transform.pos.x <= Config::World::MIN_X) {
        ball.transform.pos.x = Config::World::MIN_X;
        ball.velocity.x *= -1.0f;
    } else if (ball.transform.pos.x >= Config::World::MAX_X) {
        ball.transform.pos.x = Config::World::MAX_X;
        ball.velocity.x *= -1.0f;
    }
    if (ball.transform.pos.y >= Config::World::MAX_Y) {
        ball.transform.pos.y = Config::World::MAX_Y;
        ball.velocity.y *= -1.0f;
    }
    // Paddle Collision
    if (Math::checkAABB(paddle.transform.pos, paddle.transform.scale, ball.transform.pos, ball.radius)) {
        // paddle normal
        glm::vec3 normal(0.0f, 1.0f, 0.0f);

        ball.velocity = Math::reflectVector(ball.velocity, normal);
        // (spin)
        ball.velocity.x += paddleVelocityX * 0.12f; //todo fix magic n
        // move outside of col
        ball.transform.pos.y = paddle.transform.pos.y + paddle.transform.scale.y * 0.5f + ball.radius + 0.1f;

        ball.velocity *= Config::Ball::SPEEDUP_FACTOR;
        if (glm::length(ball.velocity) > Config::Ball::MAX_SPEED)
            ball.velocity = glm::normalize(ball.velocity) * Config::Ball::MAX_SPEED;
    }
    // Brick Collision
    for (auto& brick : bricks) {
        if (!brick.alive) continue;
        if (Math::checkAABB(brick.transform.pos, brick.transform.scale, ball.transform.pos, ball.radius)) {
            brick.alive = false;
            score += Config::Stats::SCORE_PER_BRICK;

            // determine approximate collision normal by comparing delta
            glm::vec3 delta = ball.transform.pos - brick.transform.pos;
            glm::vec3 normal;

            if (std::abs(delta.x) > std::abs(delta.y)) {
                normal = glm::vec3(delta.x > 0 ? 1.0f : -1.0f, 0.0f, 0.0f);
            } else {
                normal = glm::vec3(0.0f, delta.y > 0 ? 1.0f : -1.0f, 0.0f);
            }

            ball.velocity = Math::reflectVector(ball.velocity, normal);
            //GOAL speedup
            ball.velocity *= Config::Ball::SPEEDUP_FACTOR;
            float len = glm::length(ball.velocity);
            if (len > Config::Ball::MAX_SPEED)
                ball.velocity = glm::normalize(ball.velocity) * Config::Ball::MAX_SPEED;
            break;  //stop after one
        }
    }

    //GOAL Death
    if (ball.transform.pos.y < Config::World::MIN_Y) {
        lives--;
        if (lives <= 0) {
            gameOver = true;
            ballLaunched = false;
        } else {
            resetBallPaddle();
        }
    }

    //GOAL Victory
    bool allDestroyed = true;
    for (const auto& brick : bricks) {
        if (brick.alive) { allDestroyed = false; break; }
    }
    if (allDestroyed) {
        gameWon = true;
        gameOver = true;
        ballLaunched = false;
    }
}

void Game::render() {

    uboCamera->bind();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
    uboCamera->unbind();
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();

    renderMatrices.clear();
    renderColors.clear();
    // Active only
    for (const auto& brick : bricks) {
        if (brick.alive) {
            renderMatrices.push_back(brick.cachedMatrix);
            renderColors.push_back(brick.color);
        }
    }

    //Render Brick
    if (!renderMatrices.empty()) {
        vboInstance->setSubData(renderMatrices);
        vboColor->setSubData(renderColors);
        cubeMesh->drawInstanced((GLsizei)renderMatrices.size());
    }

    //Render Paddle + Ball
    renderMatrices.clear();
    renderColors.clear();
    renderMatrices.push_back(paddle.transform.getMatrix());
    renderColors.push_back(paddle.color);

    vboInstance->setSubDataSingle(renderMatrices[0]);
    vboColor->setSubDataSingle(renderColors[0]);
    cubeMesh->drawInstanced(1);

    renderMatrices[0] = ball.transform.getMatrix();
    renderColors[0] = ball.color;
    vboInstance->setSubDataSingle(renderMatrices[0]);
    vboColor->setSubDataSingle(renderColors[0]);
    sphereMesh->drawInstanced(1);
}

void Game::renderUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    stats.drawUI();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::Begin("GameInfo", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    ImGui::SetWindowFontScale(1.5f);
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Score: %d", score);
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Lives: %d", lives);
    ImGui::End();


    if (gameOver && !gameWon) {
        ImGui::OpenPopup("GameOver");
    }

    if (ImGui::BeginPopupModal("GameOver", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("GAME OVER");
        ImGui::Text("Final Score: %d", score);
        if (ImGui::Button("Restart", ImVec2(120, 0))) {
            resetGame();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Exit", ImVec2(120, 0))) {
            glfwSetWindowShouldClose(window.get(), true);
        }
        ImGui::EndPopup();
    }

    if (gameWon) {
        ImGui::OpenPopup("GameWon");
    }

    if (ImGui::BeginPopupModal("GameWon", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("CONGRATULATIONS!");
        ImGui::Text("You destroyed all bricks!");
        ImGui::Text("Final Score: %d", score);
        if (ImGui::Button("Restart", ImVec2(120, 0))) {
            resetGame();
            gameWon = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Exit", ImVec2(120, 0))) {
            glfwSetWindowShouldClose(window.get(), true);
        }
        ImGui::EndPopup();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
