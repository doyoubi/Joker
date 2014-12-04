#ifndef JOKER_ROLE_ENUM_TYPE
#define JOKER_ROLE_ENUM_TYPE

namespace joker
{

    enum class RoleAction
    {
        ATTACK,
        ATTACKED,
        LEFT_RUN,
        RIGHT_RUN,
        STOP,
        JUMP,
        NOD,
        READY,
    };

    enum class RoleDirection
    {
        LEFT, RIGHT
    };

}

#endif
