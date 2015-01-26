#include <string>

#include "utils/debug.h"
#include "Role.h"

namespace joker
{
    using std::string;

    // Role
    Role::Role(RoleSprite * roleSprite)
        : _simplePhysicsBody(0, 0, 0, 0), _roleSprite(roleSprite)
    {
        CHECKNULL(roleSprite);

        _simplePhysicsBody.setWidth(50);
        _simplePhysicsBody.setHeight(150);
        _simplePhysicsBody.setCollidable(false);

        // require roleSprite->_armature initialized
        _stateManager = std::move(std::unique_ptr<StateManager>(
            new StateManager(this, IdleState::create())
            ));
    }

    Role::~Role()
    {
        _roleSprite->removeFromParent();
    }

    void Role::executeCommand(const RoleCommand & command)
    {
        _stateManager->executeCommand(command);
    }

    RoleDirection Role::getDirection() const
    {
        DEBUGCHECK(std::abs(_roleSprite->getScaleX()) == 1.0f, "abs of scale x must be 1");
        return _roleSprite->getScaleX() == 1 ? RoleDirection::RIGHT : RoleDirection::LEFT;
    }

    void Role::setDirection(RoleDirection direction)
    {
        _roleSprite->setScaleX(direction == RoleDirection::RIGHT ? 1 : -1);
    }

    void Role::setPosition(const Vec2 & position)
    {
        setPosition(position.x, position.y);
    }

    void Role::setPosition(float x, float y)
    {
        _roleSprite->setPosition(x, y);
        getPhysicsBody()->setX(x);
        getPhysicsBody()->setY(y);
    }

    Vec2 Role::getPosition() const
    {
        float x = _simplePhysicsBody.getX();
        float y = _simplePhysicsBody.getY();
        return Vec2(x, y);
    }

}
