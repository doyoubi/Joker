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
            "roleAnimation/joker/joker0.png",
            "roleAnimation/joker/joker0.plist",
            "roleAnimation/joker/joker.ExportJson"
            );
        ArmatureDataManager::getInstance()->addArmatureFileInfo(
            "roleAnimation/enemy/enemy0.png",
            "roleAnimation/enemy/enemy0.plist",
            "roleAnimation/enemy/enemy.ExportJson"
            );
    }

    Role * Role::create(const string & animationName)
    {
        // check if animation has been loaded
        CHECKNULL(ArmatureDataManager::getInstance()->getAnimationData(animationName));
        CHECKNULL(ArmatureDataManager::getInstance()->getAnimationData(animationName)->getMovement("static"));
        CHECKNULL(ArmatureDataManager::getInstance()->getAnimationData(animationName)->getMovement("run"));
        CHECKNULL(ArmatureDataManager::getInstance()->getAnimationData(animationName)->getMovement("slowDown"));
        Armature * armature = Armature::create(animationName);

        Role * role = new (std::nothrow) Role(armature);
        if (!role || !role->init())
        {
            CC_SAFE_DELETE(role);
            return nullptr;
        }
        role->autorelease();
        return role;
    }

    Role::Role(Armature * armature)
        : _armature(armature), _simplePhysicsBody(0,0,0,0)
    {
        CHECKNULL(_armature);
        addChild(_armature);
        _armature->retain();
        
        _simplePhysicsBody.setWidth(50);
        _simplePhysicsBody.setHeight(150);
        _simplePhysicsBody.setCollidable(false);

        // require this->_armature initialized
        _stateManager = std::move(std::unique_ptr<StateManager>(
            new StateManager(this, IdleState::create())
            ));
    }

    void Role::executeCommand(RoleAction command)
    {
        _stateManager->executeCommand(command);
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

    void Role::setPosition(const Vec2 & position)
    {
        setPosition(position.x, position.y);
    }

    void Role::setPosition(float x, float y)
    {
        Node::setPosition(x, y);
        getPhysicsBody()->setX(x);
        getPhysicsBody()->setY(y);
    }


}
