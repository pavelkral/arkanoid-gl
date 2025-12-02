#ifndef POWERUPSYSTEM_H
#define POWERUPSYSTEM_H
#include "../entity/entitymanager.h"


namespace Ecs {


class PowerUpSystem {
public:
    void Update(EntityManager& manager, float dt);

private:
    void ApplyEffect(EntityManager& manager, Entity paddle, PowerUpType type);
};
}

#endif // POWERUPSYSTEM_H
