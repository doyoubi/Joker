#ifndef JOKER_ENEMY_CONDUCTOR
#define JOKER_ENEMY_CONDUCTOR

#include <unordered_map>
#include <vector>

#include "BehaviorTree.h"
#include "role/Role.h"

namespace joker
{

    class EnemyConductor
    {
    public:
        std::vector<RolePtr> & getEnemyArray();
        void addEnemy(RolePtr && role);
        void removeEnemy(Role * enemy);
        void tick(RolePtr & role, const BTParam & param);

    private:
        std::unordered_map<Role*, BTNodePtr> _tree;
        std::vector<RolePtr> _enemyArray;
    };

}

#endif
