#ifndef JOKER_ROLE_ENUM_TYPE
#define JOKER_ROLE_ENUM_TYPE

namespace joker
{

    enum class RoleAction
    {
        IDLE,
        ATTACK,
        ATTACKED,
        RUN,
        STOP,
        JUMP,
        NOD,
        DEFENCE,
        DEFENCE_NOD,
        COLLIDE,
        FAST_RUN,
        ATTACK_READY,
        RETREAT,
    };

    enum class RoleDirection
    {
        LEFT, RIGHT
    };

}

#endif
