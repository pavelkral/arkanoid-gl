#include "physicssystem.h"
#include "../../utils/random.h"

void Ecs::PhysicsSystem::Update(EntityManager &manager, float dt) {
    if (manager.globalState.gameOver) return;

    Entity ballEntity = NULL_ENTITY;
    TransformComponent* ballTrans = nullptr;
    RigidbodyComponent* ballRb = nullptr;
    GameStateComponent* ballState = nullptr;
    ColliderComponent* ballCol = nullptr;

    for (auto& [e, tag] : manager.tags) {
        if (tag.type == TagType::Ball) {
            ballEntity = e;
            ballTrans = manager.getEntityComponent<TransformComponent>(e);
            ballRb = manager.getEntityComponent<RigidbodyComponent>(e);
            ballState = manager.getEntityComponent<GameStateComponent>(e);
            ballCol = manager.getEntityComponent<ColliderComponent>(e);
            break;
        }
    }

    if (!ballEntity || !ballState) return;

    if (!ballState->launched) {
        for (auto& [pe, ptag] : manager.tags) {
            if (ptag.type == TagType::Paddle) {
                auto* pTrans = manager.getEntityComponent<TransformComponent>(pe);
                if (pTrans) {
                    ballTrans->position.x = pTrans->position.x;
                    ballTrans->position.y = pTrans->position.y + pTrans->scale.y * 0.5f + ballCol->radius + 0.2f;
                }
                break;
            }
        }
        return;
    }

    ballTrans->position += ballRb->velocity * dt;

    if (ballTrans->position.x <= Config::World::MIN_X) { ballTrans->position.x = Config::World::MIN_X; ballRb->velocity.x *= -1.0f; }
    else if (ballTrans->position.x >= Config::World::MAX_X) { ballTrans->position.x = Config::World::MAX_X; ballRb->velocity.x *= -1.0f; }
    if (ballTrans->position.y >= Config::World::MAX_Y) { ballTrans->position.y = Config::World::MAX_Y; ballRb->velocity.y *= -1.0f; }

    std::vector<Entity> destroyedEntities;

    for (auto& [targetE, targetCol] : manager.colliders) {
        if (targetE == ballEntity) continue;
        // ball no colision
        if (manager.powerUps.count(targetE)) continue;

        auto* targetTrans = manager.getEntityComponent<TransformComponent>(targetE);
        if (!targetTrans) continue;

        // Sphere vs Box AABB approximation
        float r = ballCol->radius;
        float halfW = targetTrans->scale.x * 0.5f;
        float halfH = targetTrans->scale.y * 0.5f;

        bool hit = (ballTrans->position.x + r > targetTrans->position.x - halfW &&
                    ballTrans->position.x - r < targetTrans->position.x + halfW &&
                    ballTrans->position.y + r > targetTrans->position.y - halfH &&
                    ballTrans->position.y - r < targetTrans->position.y + halfH);

        if (hit) {
            TagComponent* tag = manager.getEntityComponent<TagComponent>(targetE);

            if (tag && tag->type == TagType::Paddle) {
                ballRb->velocity = reflectVector(ballRb->velocity, glm::vec3(0,1,0));
                auto* playerCtrl = manager.getEntityComponent<PlayerControlComponent>(targetE);
                if (playerCtrl) ballRb->velocity.x += playerCtrl->velocityX * 0.12f;
                ballTrans->position.y = targetTrans->position.y + targetTrans->scale.y * 0.5f + r + 0.1f;
                applySpeedup(ballRb->velocity);
            }
            else if (tag && tag->type == TagType::Brick) {
                destroyedEntities.push_back(targetE);
                manager.globalState.score += Config::Stats::SCORE_PER_BRICK;

                //SPAWN POWER-UP
                TrySpawnPowerUp(manager, targetTrans->position);

                glm::vec3 delta = ballTrans->position - targetTrans->position;
                glm::vec3 normal = (std::abs(delta.x) > std::abs(delta.y))
                                       ? glm::vec3(delta.x > 0 ? 1 : -1, 0, 0)
                                       : glm::vec3(0, delta.y > 0 ? 1 : -1, 0);
                ballRb->velocity = reflectVector(ballRb->velocity, normal);
                applySpeedup(ballRb->velocity);
            }
        }
    }
    for (auto e : destroyedEntities) manager.destroyEntity(e);
}

void Ecs::PhysicsSystem::TrySpawnPowerUp(EntityManager &manager, glm::vec3 pos) {
    if (Random::Float(0.0f, 1.0f) < Config::PowerUp::DROP_CHANCE) {
        Entity pup = manager.createEntity();
        PowerUpType type = (Random::Float(0.0f, 1.0f) > 0.5f) ? PowerUpType::EnlargePaddle : PowerUpType::ExtraLife;

        // yellow (Paddle) green (Life)
        glm::vec4 color = (type == PowerUpType::EnlargePaddle)
                              ? glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)
                              : glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

        manager.addEntityComponent(pup, TagComponent{TagType::PowerUp});
        manager.addEntityComponent(pup, TransformComponent{pos, glm::vec3(2.0f, 0.8f, 1.0f)}); // flat rect
        manager.addEntityComponent(pup, RenderComponent{powerUpMesh, color});
        manager.addEntityComponent(pup, PowerUpComponent{type});

    }
}
