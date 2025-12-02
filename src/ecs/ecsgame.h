#ifndef ECSGAME_H
#define ECSGAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

#include "../utils/stats.h"
//Gl
#include "../gl/shader.h"
#include "../gl/buffer.h"

#include "entity/entitymanager.h"
#include "system/inputsystem.h"
#include "system/powerupsystem.h"
#include "system/physicssystem.h"
#include "system/gamelogicsystem.h"
#include "system/rendersystem.h"

namespace Ecs {

class Game {
    struct WindowDeleter { void operator()(GLFWwindow* ptr) { glfwDestroyWindow(ptr); } };
    std::unique_ptr<GLFWwindow, WindowDeleter> window;

    std::unique_ptr<Shader> shader;
    std::unique_ptr<Mesh> cubeMesh;
    std::unique_ptr<Mesh> sphereMesh;
    std::unique_ptr<Buffer> uboCamera;

    EntityManager manager;
    InputSystem inputSystem;
    PhysicsSystem physicsSystem;
    RenderSystem renderSystem;
    GameLogicSystem logicSystem;
    PowerUpSystem powerUpSystem;
    Stats stats;

    glm::mat4 view, proj;

public:
    bool init();
    void run();

private:
    void initResources();
    void resetGame();
    void cleanup();
};
}
#endif // ECSGAME_H
