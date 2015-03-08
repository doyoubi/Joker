#include <string>

#include "utils/debug.h"
#include "Role.h"
#include "gameplay/BattleDirector.h"
#include "enemyState.h"

namespace joker
{
    using std::string;

    // Role
    Role::Role(BattleDirector * director, RoleSprite * roleSprite, int width, int height, float spriteScale, RoleType roleType)
        : _battleDirector(director), _simplePhysicsBody(0, 0, 0, 0), _roleSprite(roleSprite), _roleType(roleType)
    {
        CHECKNULL(roleSprite);

        _simplePhysicsBody.setWidth(width);
        _simplePhysicsBody.setHeight(height);
        _simplePhysicsBody.setCollidable(false);

        // require roleSprite->_armature initialized
        if (_roleType == RoleType::BOMB)
        {
            _stateManager = std::move(std::unique_ptr<StateManager>(
                new StateManager(this, IdleState::create())
                ));
        }
        else if (_roleType == RoleType::PLAYER || roleType == RoleType::ENEMY)
        {
            _stateManager = std::move(std::unique_ptr<StateManager>(
                new StateManager(this, EnterState::create())
                ));
        }
        else ERRORMSG("invalid role type");

        _roleSprite->setScale(spriteScale);
        if (JOKER_DEBUG_ON)
            _roleSprite->addRoleSpriteDebug(this);
    }

    Role::~Role()
    {
        if (_roleType == RoleType::ENEMY)
        {
            int debug = 0;
            debug = 1;
        }
        _roleSprite->die();
    }

    bool Role::executeCommand(const RoleCommand & command)
    {
        return _stateManager->executeCommand(command);
    }

    RoleDirection Role::getDirection() const
    {
        return _roleSprite->getDirection();
    }

    void Role::setDirection(RoleDirection direction)
    {
        _roleSprite->setDirection(direction);
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

    void Role::die()
    {
        DEBUGCHECK(!_dead, string("can't kill a dead Role: ") + string(
            _roleType == RoleType::PLAYER ? "player" :
            _roleType == RoleType::ENEMY ? "enemy" :
            _roleType == RoleType::BOMB ? "bomb" : "invalid role type"));
        _dead = true;
        if (_roleType == RoleType::PLAYER)
            ;
        else if (_roleType == RoleType::ENEMY)
            _battleDirector->addEvent(EventPtr(new RemoveRoleEvent(this)));
        else if (_roleType == RoleType::BOMB)
            _battleDirector->addEvent(EventPtr(new RemoveRoleEvent(this)));
    }

}
