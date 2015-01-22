#ifndef JOKER_ROLE_ENUM_TYPE
#define JOKER_ROLE_ENUM_TYPE

namespace joker
{

    enum class RoleAction
    {
        IDLE,
        ATTACK,
        ATTACKED,
        LEFT_RUN,
        RIGHT_RUN,
        STOP,
        JUMP,
        NOD,
        DEFENCE,
        DEFENCE_NOD,
        COLLIDE_TO_LEFT,
        COLLIDE_TO_RIGHT,
    };

    enum class RoleDirection
    {
        LEFT, RIGHT
    };

}

#endif
