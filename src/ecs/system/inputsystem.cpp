#include "inputsystem.h"
#include <algorithm>

void Ecs::InputSystem::Update(EntityManager &manager, GLFWwindow *window, float dt) {
    for (auto& [entity, playerCtrl] : manager.players) {
        auto* transform = manager.getEntityComponent<TransformComponent>(entity);
        if (!transform) continue;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float normX = (float)xpos / width;
        float worldX = Config::World::MIN_X + normX * (Config::World::MAX_X - Config::World::MIN_X);
        float prevX = transform->position.x;
        transform->position.x += (worldX - transform->position.x) * 15.0f * dt;

        // Clamp if padle scale up
        float halfW = transform->scale.x * 0.5f;
        transform->position.x = std::clamp(transform->position.x, Config::World::MIN_X + halfW, Config::World::MAX_X - halfW);

        if (dt > 0.0f) playerCtrl.velocityX = (transform->position.x - prevX) / dt;
        playerCtrl.lastX = transform->position.x;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        for (auto& [entity, state] : manager.gameStates) {
            if (manager.tags[entity].type == TagType::Ball && !state.launched && !manager.globalState.gameOver) {
                state.launched = true;
            }
        }
    }
}
