#ifndef OOPGAME_H
#define OOPGAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <memory>

#include "../config.h"
#include "../utils/stats.h"

//Gl
#include "../gl/shader.h"
#include "../gl/buffer.h"
#include "../geometry/mesh.h"

#include "gameobjects.h"

class Game {

    struct WindowDeleter {
        void operator()(GLFWwindow* ptr) {
            glfwDestroyWindow(ptr);
        }
    };
    std::unique_ptr<GLFWwindow, WindowDeleter> window;

    std::unique_ptr<Shader> shader;
    std::unique_ptr<Mesh> cubeMesh;
    std::unique_ptr<Mesh> sphereMesh;

    std::unique_ptr<Buffer> uboCamera;
    std::unique_ptr<Buffer> vboInstance;
    std::unique_ptr<Buffer> vboColor;

    std::vector<Brick> bricks;
    Paddle paddle;
    Ball ball;
    glm::mat4 view;
    glm::mat4 proj;

    bool running = true;
    bool gameOver = false;
    bool gameWon = false;
    bool ballLaunched = false;
    int score = 0;
    int lives = Config::Stats::INITIAL_LIVES;
    float fpsTimer = 0.0f;
    int frameCount = 0;
    Stats stats;

    std::vector<glm::mat4> renderMatrices;
    std::vector<glm::vec4> renderColors;
    float lastPaddleX = Config::Paddle::START_POS.x;
    float paddleVelocityX = 0.0f;

public:
    Game() = default;
    [[nodiscard]] bool init();
    void run();

private:
    void initResources();
    void resetGame();
    void resetBallPaddle();
    void processInput(float dt);

    void updatePhysics(float dt);
    void render();
    void renderUI();
    void cleanup();
};


#endif // OOPGAME_H
