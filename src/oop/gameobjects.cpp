#include "gameobjects.h"


glm::mat4 Transform::getMatrix() const {
    glm::mat4 m(1.0f);
    m = glm::translate(m, pos);
    m = glm::scale(m, scale);
    return m;
}

Brick::Brick(glm::vec3 p, glm::vec3 s, glm::vec4 c) : color(c) {
    transform.pos = p;
    transform.scale = s;
    cachedMatrix = transform.getMatrix();
}
