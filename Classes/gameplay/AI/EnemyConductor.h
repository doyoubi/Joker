#ifndef JOKER_ENEMY_CONDUCTOR
#define JOKER_ENEMY_CONDUCTOR

#include <unordered_map>
#include <vector>

#include "BehaviorTree.h"

namespace joker
{
    class Role;

    class EnemyConductor
    {
    public:
        const std::vector<Role*> & getEnemyArray() const;
        void addEnemy(Role * role);
        void removeEnemy(Role * enemy);
        void tick(Role * role, const BTParam & param);

    private:
        std::unordered_map<Role*, BTNodePtr> _tree;
        std::vector<Role*> _enemyArray;
    };
}

#endif
