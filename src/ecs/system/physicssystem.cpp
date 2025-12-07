#include "physicssystem.h"
#include "../../utils/random.h"
#include "../../utils/math.h"

void Ecs::PhysicsSystem::Update(EntityManager &manager, float dt) {
    if (manager.globalState.gameOver) return;

    Entity ballEntity = NULL_ENTITY;
    TransformComponent* ballTrans = nullptr;
    RigidbodyComponent* ballRb = nullptr;
    GameStateComponent* ballState = nullptr;
    ColliderComponent* ballCol = nullptr;
    // Najdeme míèek
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

    // (Sticky ball)
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
    // Pohyb míèku
    ballTrans->position += ballRb->velocity * dt;
    // Odraz od stìn (Walls)
    if (ballTrans->position.x <= Config::World::MIN_X) {
        ballTrans->position.x = Config::World::MIN_X;
        ballRb->velocity.x *= -1.0f;
    }
    else if (ballTrans->position.x >= Config::World::MAX_X) {
        ballTrans->position.x = Config::World::MAX_X;
        ballRb->velocity.x *= -1.0f;
    }
    if (ballTrans->position.y >= Config::World::MAX_Y) {
        ballTrans->position.y = Config::World::MAX_Y;
        ballRb->velocity.y *= -1.0f;
    }
    //  (Paddle, Bricks)
    std::vector<Entity> destroyedEntities;

    for (auto& [targetE, targetCol] : manager.colliders) {
        if (targetE == ballEntity) continue;
        if (manager.powerUps.count(targetE)) continue; // Ignore powerups

        auto* targetTrans = manager.getEntityComponent<TransformComponent>(targetE);
        if (!targetTrans) continue;

        float r = ballCol->radius;
        float halfW = targetTrans->scale.x * 0.5f;
        float halfH = targetTrans->scale.y * 0.5f;

        bool hit = Math::checkAABB(targetTrans->position, targetTrans->scale, ballTrans->position, r);
        
        if (hit) {
            TagComponent* tag = manager.getEntityComponent<TagComponent>(targetE);

            // --- PADDLE ---
            if (tag && tag->type == TagType::Paddle) {
                // 
                ballRb->velocity.y = abs(ballRb->velocity.y); //up
 
                auto* playerCtrl = manager.getEntityComponent<PlayerControlComponent>(targetE);
                if (playerCtrl) ballRb->velocity.x += playerCtrl->velocityX * 0.12f;

				// unpenetrate the ball
                ballTrans->position.y = targetTrans->position.y + halfH + r + 0.05f;
                applySpeedup(ballRb->velocity);
            }

            //fix tuneling
            else if (tag && tag->type == TagType::Brick) {
                destroyedEntities.push_back(targetE);
                manager.globalState.score += Config::Stats::SCORE_PER_BRICK;
                TrySpawnPowerUp(manager, targetTrans->position);

				// depth of penetration
                float deltaX = ballTrans->position.x - targetTrans->position.x;
                float deltaY = ballTrans->position.y - targetTrans->position.y;
				//depth of penetration in each axis
                float intersectX = abs(deltaX) - (halfW + r);
                float intersectY = abs(deltaY) - (halfH + r);
				//  intersection x is smaller than y vertical intersection
                if (intersectX > intersectY) {
					// horizontal collision
					// push the ball out in X axis
                    if (deltaX > 0.0f)
                        ballTrans->position.x = targetTrans->position.x + halfW + r;
                    else
                        ballTrans->position.x = targetTrans->position.x - halfW - r;
                    ballRb->velocity.x *= -1.0f; // Odraz X
                }
                else {
					// vertical collision
					// push the ball out in Y axis
                    if (deltaY > 0.0f)
                        ballTrans->position.y = targetTrans->position.y + halfH + r;
                    else
                        ballTrans->position.y = targetTrans->position.y - halfH - r;

                    ballRb->velocity.y *= -1.0f; //  Y
                }
                applySpeedup(ballRb->velocity);

                break;
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
