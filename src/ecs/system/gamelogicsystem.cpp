#include "gamelogicsystem.h"

void Ecs::GameLogicSystem::Update(EntityManager &manager) {
    Entity ballEntity = NULL_ENTITY;
    for (auto& [e, tag] : manager.tags) if (tag.type == TagType::Ball) ballEntity = e;

    if (ballEntity != NULL_ENTITY) {
        auto* trans = manager.getEntityComponent<TransformComponent>(ballEntity);
        if (trans && trans->position.y < Config::World::MIN_Y) {
            manager.globalState.lives--;
            if (manager.globalState.lives <= 0) {
                manager.globalState.gameOver = true;
            } else {
                resetRound(manager, ballEntity);
            }
        }
    }
    bool anyBrick = false;
    for (auto& [e, tag] : manager.tags) { if (tag.type == TagType::Brick) { anyBrick = true; break; } }
    if (!anyBrick) { manager.globalState.gameWon = true; manager.globalState.gameOver = true; }
}

void Ecs::GameLogicSystem::resetRound(EntityManager &manager, Entity ball) {
    auto* ballState = manager.getEntityComponent<GameStateComponent>(ball);
    auto* ballRb = manager.getEntityComponent<RigidbodyComponent>(ball);

    if (ballState) ballState->launched = false;
    if (ballRb) ballRb->velocity = Config::Ball::START_VEL;

    // Reset Paddle Position and Scale
    for(auto& [e, tag] : manager.tags) {
        if(tag.type == TagType::Paddle) {
            auto* pt = manager.getEntityComponent<TransformComponent>(e);
            if(pt) {
                pt->position = Config::Paddle::START_POS;
                pt->scale = Config::Paddle::SCALE; // Reset size
            }
            auto* pc = manager.getEntityComponent<PlayerControlComponent>(e);
            if(pc) { pc->velocityX = 0; pc->lastX = Config::Paddle::START_POS.x; }
        }
    }

    //  powerupy reset
    std::vector<Entity> toRemove;
    for(auto& [e, p] : manager.powerUps) toRemove.push_back(e);
    for(auto e : toRemove) manager.destroyEntity(e);
}
