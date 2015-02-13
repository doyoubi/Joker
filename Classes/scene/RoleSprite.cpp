#include "RoleSprite.h"
#include "utils/debug.h"
#include "utils/RoleStateDebug.h"

namespace joker
{
    using namespace cocos2d;
    using namespace cocostudio;
    using std::string;

    void RoleSprite::loadAnimationSource()
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

    RoleSprite * RoleSprite::create(const string & animationName, string animationDirection)
    {
        // check if animation has been loaded
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(animationName), "missing animation: " + animationName);
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(animationName)->getMovement("static"),
            "mising 'static' movement");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(animationName)->getMovement("run"),
            "mising 'run' movement");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(animationName)->getMovement("slowDown"),
            "mising 'slowDown' movement");
        Armature * armature = Armature::create(animationName);

        RoleSprite * role = new (std::nothrow) RoleSprite(armature, animationDirection);
        if (!role || !role->init())
        {
            CC_SAFE_DELETE(role);
            return nullptr;
        }
        role->autorelease();
        return role;
    }

    RoleSprite::RoleSprite(Armature * armature, string animationDirection)
        : _armature(armature), _animationDirection(animationDirection == "left" ? -1 : 1)
    {
        CHECKNULL(_armature);
        DEBUGCHECK(animationDirection == "left" || animationDirection == "right",
            "animationDirection must be either 'left' or 'right'.");
        addChild(_armature);
        _armature->retain();
    }

    void RoleSprite::die()
    {
        DEBUGCHECK(getArmature()->getAnimation()->getAnimationData()->getMovement("attacked"),
            "missing 'attacked' movement.");
        DEBUGCHECK(getArmature()->getAnimation()->getAnimationData()->getMovement("dead"),
            "missing 'dead' movement.");
        std::vector<std::string> names = { "attacked", "dead" };
        getArmature()->getAnimation()->playWithNames(names, -1, false);
        getArmature()->getAnimation()->setMovementEventCallFunc(
            [this](Armature *armature, MovementEventType movementType, const std::string& movementID){
            if (movementType == MovementEventType::LOOP_COMPLETE)
            {
                this->removeFromParent();
            }
        });
        removeRoleSpriteDebug();
    }

    void RoleSprite::setDirection(RoleDirection direction)
    {
        float sx = std::abs(_armature->getScaleX()) * _animationDirection;
        if (direction == RoleDirection::LEFT) sx *= -1;
        _armature->setScaleX(sx);
    }

    RoleDirection RoleSprite::getDirection() const
    {
        return _armature->getScaleX() * _animationDirection > 0 ? RoleDirection::RIGHT : RoleDirection::LEFT;
    }

    void RoleSprite::addRoleSpriteDebug(Role * role)
    {
        auto debugMsg = RoleStateDebug::create();
        debugMsg->setRole(role);
        debugMsg->setName("RoleSpriteDebug");
        this->addChild(debugMsg);
    }

    void RoleSprite::removeRoleSpriteDebug()
    {
        this->removeChildByName("RoleSpriteDebug");
    }

}
