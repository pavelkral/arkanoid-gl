#ifndef GAMELOGICSYSTEM_H
#define GAMELOGICSYSTEM_H
#include "../entity/entitymanager.h"

namespace Ecs {
class GameLogicSystem {
public:
    void Update(EntityManager& manager);

    void resetRound(EntityManager& manager, Entity ball);
};

}

#endif // GAMELOGICSYSTEM_H
