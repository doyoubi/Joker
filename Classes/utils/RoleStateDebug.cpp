#include "RoleStateDebug.h"
#include "utils/debug.h"
#include "role/Role.h"

namespace joker
{
    bool RoleStateDebug::init()
    {
        if (!Node::init()) return false;

        const int fontSize = 35;
        const int lineHeight = fontSize + 10;
        _currState = Label::createWithTTF("current state", "fonts/Marker Felt.ttf", fontSize);
        _direction = Label::createWithTTF("direction", "fonts/Marker Felt.ttf", fontSize);
        addChild(_currState);
        addChild(_direction);
        _direction->setPositionY(lineHeight);

        getScheduler()->scheduleUpdate(this, 0, false);
        return true;
    }

    RoleStateDebug::~RoleStateDebug()
    {
        getScheduler()->unscheduleUpdate(this);
    }

    void RoleStateDebug::setRole(Role * role)
    {
        CHECKNULL(role);
        _role = role;
        float height = role->getRoleSprite()->getContentSize().height;
        this->setPositionY(height);
    }

    void RoleStateDebug::update(float dt)
    {
        string stateMsg = _role->getStateManager()->getDebugString();
        _currState->setString(stateMsg);
        string direction = _role->getDirection() == RoleDirection::LEFT ? "@-----" : "-----@";
        _direction->setString(direction);
    }
}
