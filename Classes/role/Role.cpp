#include <string>

#include "utils/debug.h"
#include "Role.h"

namespace joker
{
    using namespace cocos2d;
    using namespace cocostudio;
    using std::string;

    // Role
    void Role::loadAnimationSource()
    {
        ArmatureDataManager::getInstance()->addArmatureFileInfo(
            "joker/joker0.png",
            "joker/joker0.plist",
            "joker/joker.ExportJson"
            );
    }

    Role * Role::create(const string & animationName)
    {
        CHECKNULL(ArmatureDataManager::getInstance()->getAnimationData(animationName));
        Armature * armature = Armature::create(animationName);

        Role * role = new (std::nothrow) Role(armature);
        if (!role || !role->init())
        {
            CC_SAFE_DELETE(role);
            return nullptr;
        }
        role->autorelease();
    }

    Role::Role(Armature * armature)
        : _stateManager(this, StatePtr(new IdleState())),
        // warning: StateManager should only store 'this' pointer
        // and should not access it's member data or member function
        // and Role should not be used in multiple inheritance
        _armature(armature)
    {
    }

    void Role::receiveCommand(RoleAction command)
    {
        _stateManager.executeCommand(command);
    }

    RoleDirection Role::getDirection() const
    {
        DEBUGCHECK(std::abs(getScaleX()) == 1.0f, "abs of scale x must be 1");
        return getScaleX() == 1 ? RoleDirection::RIGHT : RoleDirection::LEFT;
    }

    void Role::setDirection(RoleDirection direction)
    {
        setScaleX(direction == RoleDirection::RIGHT ? 1 : -1);
    }

}
