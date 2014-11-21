#ifndef JOKER_ROLE
#define JOKER_ROLE

#include <string>

#include "cocos2d.h"
#include "cocostudio\CCArmature.h"

#include "state.h"

namespace joker
{

    enum class RoleAction
    {
        ATTACK,
        ATTACKED,
        LEFT_RUN,
        RIGHT_RUN,
    };


    enum class RoleDirection
    {
        LEFT, RIGHT
    };


    class Role : public cocos2d::Node
    {
    public:
        static Role * create(const std::string & animationFile);
        Role();

        void receiveCommand(RoleAction command);
        RoleDirection getDirection();
        void setDirection(RoleDirection direction);
        cocostudio::Armature * getArmature();

    private:
        StateManager _stateManager;
        cocostudio::Armature * _armature;
    };


}

#endif
