#ifndef JOKER_BATTLE_DIRECTOR
#define JOKER_BATTLE_DIRECTOR

#include "role/RoleEnumType.h"

namespace joker
{

    class Role;
    class BattleScene;

    class BattleDirector
    {
    public:
        BattleDirector(BattleScene * battleScene);
        void sendCommand(Role * role, RoleAction command);
        Role * getPlayer();
    private:
        BattleScene * _battleScene = nullptr;   // weak reference
    };

}

#endif
