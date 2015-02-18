#ifndef JOKER_ROLE_ENUM_TYPE
#define JOKER_ROLE_ENUM_TYPE

namespace joker
{

    enum class RoleAction
    {
        IDLE,
        ATTACK,
        EMPTY_ATTACK,
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
        EXPLODE,
        FALLING,
    };

    enum class RoleDirection
    {
        LEFT, RIGHT
    };

}

#endif
