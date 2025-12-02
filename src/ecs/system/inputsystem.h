#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../entity/entitymanager.h"

namespace Ecs {

class InputSystem {
public:
    void Update(EntityManager& manager, GLFWwindow* window, float dt);
};

}

#endif // INPUTSYSTEM_H
