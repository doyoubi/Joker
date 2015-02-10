#include <string>

#include "utils/debug.h"
#include "Role.h"

namespace joker
{
    using std::string;

    // Role
    Role::Role(RoleSprite * roleSprite, int width, int height, float spriteScale)
        : _simplePhysicsBody(0, 0, 0, 0), _roleSprite(roleSprite)
    {
        CHECKNULL(roleSprite);

        _simplePhysicsBody.setWidth(width);
        _simplePhysicsBody.setHeight(height);
        _simplePhysicsBody.setCollidable(false);

        // require roleSprite->_armature initialized
        _stateManager = std::move(std::unique_ptr<StateManager>(
            new StateManager(this, IdleState::create())
            ));

        _roleSprite->setScale(spriteScale);
    }

    Role::~Role()
    {
        _roleSprite->die();
    }

    void Role::executeCommand(const RoleCommand & command)
    {
        _stateManager->executeCommand(command);
    }

    RoleDirection Role::getDirection() const
    {
        //DEBUGCHECK(std::abs(_roleSprite->getScaleX()) == 1.0f, "abs of scale x must be 1");
        return _roleSprite->getScaleX() > 0 ? RoleDirection::RIGHT : RoleDirection::LEFT;
    }

    void Role::setDirection(RoleDirection direction)
    {
        float sx = std::abs(_roleSprite->getScaleX());
        if (direction == RoleDirection::LEFT) sx *= -1;
        _roleSprite->setScaleX(sx);
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
