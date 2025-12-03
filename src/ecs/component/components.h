#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../../geometry/mesh.h"

namespace Ecs {

enum class TagType { None, Paddle, Ball, Brick, PowerUp };

enum class PowerUpType { EnlargePaddle, ExtraLife };

struct PowerUpComponent {
    PowerUpType type;
};

struct TagComponent { 
    TagType type; 
};

struct TransformComponent {
    glm::vec3 position{0.0f};
    glm::vec3 scale{1.0f};
    glm::mat4 getMatrix() const {
        glm::mat4 m(1.0f);
        m = glm::translate(m, position);
        m = glm::scale(m, scale);
        return m;
    }
};

struct RigidbodyComponent { 
    glm::vec3 velocity{0.0f}; 
};

struct ColliderComponent {
    enum Type { Box, Sphere } type;
    float radius = 1.0f;
};

struct RenderComponent {
    Mesh* mesh = nullptr;
    glm::vec4 color{1.0f};
    bool visible = true;
};

struct PlayerControlComponent { 
    float lastX = 0.0f; 
    float velocityX = 0.0f; 
};

struct GameStateComponent { 
    bool launched = false; 
};

}

#endif // COMPONENTS_H
