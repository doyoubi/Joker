#include <algorithm>

#include "EnemyConductor.h"
#include "utils/debug.h"

namespace joker
{

    // EnemyConductor
    const std::vector<Role*> & EnemyConductor::getEnemyArray() const
    {
        return _enemyArray;
    }

    void EnemyConductor::addEnemy(Role * enemy)
    {
        CHECKNULL(enemy);
        _tree.emplace(enemy, createEnemyTree(enemy));
        _enemyArray.push_back(enemy);
    }

    void EnemyConductor::removeEnemy(Role * enemy)
    {
        CHECKNULL(enemy);
        DEBUGCHECK(_tree.count(enemy) == 1, "enemy not exist in EnemyConductor");
        auto it = std::find(begin(_enemyArray), end(_enemyArray), enemy);
        DEBUGCHECK(end(_enemyArray) != it, "enemy not exist in EnemyConductor");
        _tree.erase(enemy);
        _enemyArray.erase(it);
    }

    void EnemyConductor::tick(Role * role, const BTParam & param)
    {
        CHECKNULL(role);
        DEBUGCHECK(end(_enemyArray) != std::find(begin(_enemyArray), end(_enemyArray), role),
            "enemy not exist in EnemyConductor");
        DEBUGCHECK(_tree.count(role) == 1, "enemy not exist in EnemyConductor");
        _tree.at(role)->tick(param);
    }

}
