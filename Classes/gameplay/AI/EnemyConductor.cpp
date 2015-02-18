#include <algorithm>

#include "EnemyConductor.h"
#include "utils/debug.h"

namespace joker
{

    // EnemyConductor
    std::vector<Role*> EnemyConductor::getEnemyArray()
    {
        std::vector<Role*> ret;
        for (auto & ptr : _enemyArray) ret.push_back(ptr.get());
        return ret;
    }

    void EnemyConductor::addEnemy(RolePtr && enemy)
    {
        CHECKNULL(enemy);
        _tree.emplace(enemy.get(), createEnemyTree(enemy.get()));
        _enemyArray.push_back(std::move(enemy));
    }

    void EnemyConductor::removeEnemy(Role * enemy)
    {
        CHECKNULL(enemy);
        DEBUGCHECK(_tree.count(enemy) == 1, "enemy not exist in EnemyConductor");
        auto it = std::find_if(begin(_enemyArray), end(_enemyArray), [enemy](RolePtr & role){ return role.get() == enemy; });
        DEBUGCHECK(end(_enemyArray) != it, "enemy not exist in EnemyConductor");
        _tree.erase(enemy);
        _enemyArray.erase(it);
    }

    void EnemyConductor::tick(Role * role, const BTParam & param)
    {
        CHECKNULL(role);
        DEBUGCHECK(end(_enemyArray) != std::find_if(begin(_enemyArray), end(_enemyArray),
            [role](RolePtr & ptr){ return ptr.get() == role; }),
            "enemy not exist in EnemyConductor");
        DEBUGCHECK(_tree.count(role) == 1, "enemy not exist in EnemyConductor");
        _tree.at(role)->tick(param);
    }

}
