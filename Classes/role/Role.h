#ifndef JOKER_ROLE
#define JOKER_ROLE

#include <string>

#include "cocos2d.h"
#include "cocostudio\CCArmature.h"

#include "state.h"
#include "RoleEnumType.h"

namespace joker
{

    class Role : public cocos2d::Node
    {
    public:
        // load animation project to ArmatureDataManager
        // should be called once before using Role::create
        static void loadAnimationSource();
        static Role * create(const std::string & animationName);

        void receiveCommand(RoleAction command);
        RoleDirection getDirection() const;
        void setDirection(RoleDirection direction);
        cocostudio::Armature * getArmature() { return _armature; }

    private:
        Role(cocostudio::Armature * armature);

        StateManager _stateManager;
        cocostudio::Armature * _armature;
    };


}

#endif
