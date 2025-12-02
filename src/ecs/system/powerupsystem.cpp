#include "powerupsystem.h"
#include "../../utils/math.h"


void Ecs::PowerUpSystem::Update(EntityManager &manager, float dt) {

    // find paddle
    Entity paddle = NULL_ENTITY;
    TransformComponent* paddleTrans = nullptr;
    for(auto& [e, tag] : manager.tags) {
        if (tag.type == TagType::Paddle) {
            paddle = e;
            paddleTrans = manager.getEntityComponent<TransformComponent>(e);
            break;
        }
    }

    std::vector<Entity> toDestroy;

    // iterate PowerUps
    for (auto& [e, pup] : manager.powerUps) {
        auto* trans = manager.getEntityComponent<TransformComponent>(e);

        //fall down
        trans->position.y -= Config::PowerUp::FALL_SPEED * dt;

        if (paddleTrans) {
            if (Math::checkBoxVsBoxAABB(paddleTrans->position, paddleTrans->scale, trans->position, trans->scale)) {
                ApplyEffect(manager, paddle, pup.type);
                toDestroy.push_back(e);
                continue;
            }
        }

        // remove if lost
        if (trans->position.y < Config::World::MIN_Y) {
            toDestroy.push_back(e);
        }
    }

    for(auto e : toDestroy) manager.destroyEntity(e);
}

void Ecs::PowerUpSystem::ApplyEffect(EntityManager &manager, Entity paddle, PowerUpType type) {
    if (type == PowerUpType::EnlargePaddle) {
        auto* trans = manager.getEntityComponent<TransformComponent>(paddle);
        if (trans) {
            trans->scale.x *= 1.3f; // o 30% fix mn
            // max size lim
            if (trans->scale.x > 20.0f) trans->scale.x = 20.0f;
        }
    } else if (type == PowerUpType::ExtraLife) {
        manager.globalState.lives++;
    }
}
