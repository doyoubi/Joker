#include "EnemyConductor.h"
#include "utils/debug.h"

namespace joker
{

    // EnemyConductor
    void EnemyConductor::addEnemy(Role * enemy)
    {
        CHECKNULL(enemy);
        _tree.emplace(enemy, createEnemyTree(enemy));
    }

    void EnemyConductor::removeEnemy(Role * enemy)
    {
        CHECKNULL(enemy);
        DEBUGCHECK(_tree.count(enemy) == 1, "enemy not exist in EnemyConductor");
        _tree.erase(enemy);
    }

    void EnemyConductor::tick(Role * role, const BTParam & param)
    {
        CHECKNULL(role);
        _tree.at(role)->tick(param);
    }
}
