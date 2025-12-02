#include "entitymanager.h"

Ecs::Entity Ecs::EntityManager::createEntity() {
    Entity id = nextId++;
    entities.push_back(id);
    return id;
}

void Ecs::EntityManager::destroyEntity(Entity e) {
    entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
    tags.erase(e);
    transforms.erase(e);
    rigidbodies.erase(e);
    colliders.erase(e);
    renderables.erase(e);
    players.erase(e);
    gameStates.erase(e);
    powerUps.erase(e);
}

void Ecs::EntityManager::clear() {
    entities.clear();
    tags.clear();
    transforms.clear();
    rigidbodies.clear();
    colliders.clear();
    renderables.clear();
    players.clear();
    gameStates.clear();
    powerUps.clear();
    nextId = 1;
}
