#ifndef JOKER_ROLE_STATE_DEBUG
#define JOKER_ROLE_STATE_DEBUG

#include "cocos2d.h"

namespace joker
{
    class Role;

    class RoleStateDebug : public cocos2d::Node
    {
    public:
        CREATE_FUNC(RoleStateDebug);
        void setRole(Role * role);
        void update(float dt);

        ~RoleStateDebug();
    private:
        bool init();

        Role * _role;
        cocos2d::Label * _currState;
        cocos2d::Label * _direction;
    };
}

#endif
