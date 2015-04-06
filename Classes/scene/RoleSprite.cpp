#include "RoleSprite.h"
#include "utils/debug.h"
#include "utils/RoleStateDebug.h"
#include "utils/config.h"

namespace joker
{
    using namespace cocos2d;
    using namespace cocostudio;
    using std::string;

    RoleSprite * RoleSprite::create(const string & animationName, const string & exportJsonFile, string animationDirection)
    {
        AnimationSprite * s = AnimationSprite::create(animationName, exportJsonFile);

        RoleSprite * role = new (std::nothrow) RoleSprite(s, animationDirection);
        if (!role || !role->init())
        {
            CC_SAFE_DELETE(role);
            return nullptr;
        }
        role->autorelease();
        return role;
    }

    RoleSprite::RoleSprite(AnimationSprite * animationSprite, string animationDirection)
        : _animationSprite(animationSprite), _animationDirection(animationDirection == "left" ? -1 : 1)
    {
        CHECKNULL(_animationSprite);
        DEBUGCHECK(animationDirection == "left" || animationDirection == "right",
            "animationDirection must be either 'left' or 'right'.");
        addChild(_animationSprite);
        _animationSprite->retain();
    }

    void RoleSprite::die()
    {
        auto missingAnimation = [](const string & animName) { return "RoleSprite::die(): missing '" + animName + "' movement."; };
        static const string deadAnimName = Config::getInstance().getStringValue({ "animation", "role", "dead" });
        _animationSprite->playAnimAction(deadAnimName);
        _animationSprite->setActionCompleteCallback("dead", [this](){
            if (this->deadCallback)
                deadCallback();
            this->removeFromParent();
        });
        if (JOKER_DEBUG_ON)
            removeRoleSpriteDebug();
    }

    void RoleSprite::setDirection(RoleDirection direction)
    {
        float sx = std::abs(_animationSprite->getScaleX()) * _animationDirection;
        if (direction == RoleDirection::LEFT) sx *= -1;
        _animationSprite->setScaleX(sx);
    }

    RoleDirection RoleSprite::getDirection() const
    {
        return _animationSprite->getScaleX() * _animationDirection > 0 ? RoleDirection::RIGHT : RoleDirection::LEFT;
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
