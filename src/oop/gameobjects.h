#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Transform {
    glm::vec3 pos{0.0f};
    glm::vec3 scale{1.0f};

    glm::mat4 getMatrix() const;
};

struct Brick {
    Transform transform;
    glm::vec4 color;
    glm::mat4 cachedMatrix;
    bool alive = true;

    Brick(glm::vec3 p, glm::vec3 s, glm::vec4 c);
};

struct Paddle {
    Transform transform;
    glm::vec4 color{0.3f, 0.8f, 0.3f, 1.0f};
};

struct Ball {
    Transform transform;
    glm::vec3 velocity;
    float radius;
    glm::vec4 color{1.0f, 0.2f, 0.2f, 1.0f};
};

#endif // GAMEOBJECTS_H
