#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <vector>
#include <unordered_map>
#include <type_traits> // if constexpr (C++17)
#include "../component/components.h"
#include "../../config.h"

namespace Ecs {
using Entity = std::uint32_t;
const Entity NULL_ENTITY = 0;

class EntityManager {
public:
    std::vector<Entity> entities;
    std::uint32_t nextId = 1;

    std::unordered_map<Entity, TagComponent> tags;
    std::unordered_map<Entity, TransformComponent> transforms;
    std::unordered_map<Entity, RigidbodyComponent> rigidbodies;
    std::unordered_map<Entity, ColliderComponent> colliders;
    std::unordered_map<Entity, RenderComponent> renderables;
    std::unordered_map<Entity, PlayerControlComponent> players;
    std::unordered_map<Entity, GameStateComponent> gameStates;
    std::unordered_map<Entity, PowerUpComponent> powerUps;

    struct {
        int score = 0;
        int lives = Config::Stats::INITIAL_LIVES;
        bool gameOver = false;
        bool gameWon = false;
    } globalState;

    Entity createEntity();

    void destroyEntity(Entity e);

    void clear();

    template<typename T>
    T& addEntityComponent(Entity e, T component) {

        if constexpr (std::is_same_v<T, TagComponent>) {
            return tags[e] = component;
        } else if constexpr (std::is_same_v<T, TransformComponent>) {
            return transforms[e] = component;
        } else if constexpr (std::is_same_v<T, RigidbodyComponent>) {
            return rigidbodies[e] = component;
        } else if constexpr (std::is_same_v<T, ColliderComponent>) {
            return colliders[e] = component;
        } else if constexpr (std::is_same_v<T, RenderComponent>) {
            return renderables[e] = component;
        } else if constexpr (std::is_same_v<T, PlayerControlComponent>) {
            return players[e] = component;
        } else if constexpr (std::is_same_v<T, GameStateComponent>) {
            return gameStates[e] = component;
        } else if constexpr (std::is_same_v<T, PowerUpComponent>) {
            return powerUps[e] = component;
        } else {
            static_assert(std::is_same_v<T, T> && false, "Attempted to add an unhandled component type.");
        }
    }

    template<typename T>
    T* getEntityComponent(Entity e) {

        if constexpr (std::is_same_v<T, TagComponent>) {
            return tags.count(e) ? &tags.at(e) : nullptr;
        } else if constexpr (std::is_same_v<T, TransformComponent>) {
            return transforms.count(e) ? &transforms.at(e) : nullptr;
        } else if constexpr (std::is_same_v<T, RigidbodyComponent>) {
            return rigidbodies.count(e) ? &rigidbodies.at(e) : nullptr;
        } else if constexpr (std::is_same_v<T, ColliderComponent>) {
            return colliders.count(e) ? &colliders.at(e) : nullptr;
        } else if constexpr (std::is_same_v<T, RenderComponent>) {
            return renderables.count(e) ? &renderables.at(e) : nullptr;
        } else if constexpr (std::is_same_v<T, PlayerControlComponent>) {
            return players.count(e) ? &players.at(e) : nullptr;
        } else if constexpr (std::is_same_v<T, GameStateComponent>) {
            return gameStates.count(e) ? &gameStates.at(e) : nullptr;
        } else if constexpr (std::is_same_v<T, PowerUpComponent>) {
            return powerUps.count(e) ? &powerUps.at(e) : nullptr;
        } else {
            return nullptr;
        }
    }
};

} // namespace Ecs

#endif // ENTITYMANAGER_H
