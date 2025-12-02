#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H
#include "../entity/entitymanager.h"


namespace Ecs {
class PhysicsSystem {
public:
    //  reference mesh for spawn powerup
    Mesh* powerUpMesh = nullptr;

    void Update(EntityManager& manager, float dt);

private:
    void TrySpawnPowerUp(EntityManager& manager, glm::vec3 pos);

    glm::vec3 reflectVector(const glm::vec3& v, const glm::vec3& normal) { return v - 2.0f * glm::dot(v, normal) * normal; }
    void applySpeedup(glm::vec3& vel) {
        vel *= Config::Ball::SPEEDUP_FACTOR;
        if (glm::length(vel) > Config::Ball::MAX_SPEED) vel = glm::normalize(vel) * Config::Ball::MAX_SPEED;
    }
};
}

#endif // PHYSICSSYSTEM_H
