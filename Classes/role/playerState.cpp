#include "playerState.h"
#include "Role.h"
#include "SimplePhysics/PhysicsWorld.h"

namespace joker
{

    // PlayerAttackState
    void PlayerAttackState::enterState(Role * role)
    {
        role->getArmature()->getAnimation()->play("attack");
        const float changedDistance = 200;
        float d = role->getDirection() == RoleDirection::LEFT ? -changedDistance : changedDistance;
        role->getPhysicsBody()->setX(d + role->getPhysicsBody()->getX());
    }

    void PlayerAttackState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }


    // PlayerAttackedState
    void PlayerAttackedState::enterState(Role * role)
    {
        role->getArmature()->getAnimation()->play("attacked");
    }

    void PlayerAttackedState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }


    // JumpState
    const float JumpState::speedX = 50;

    JumpState::JumpState(float velocityX)
        : _velocityX(velocityX)
    {
    }

    void JumpState::enterState(Role * role)
    {
        role->getArmature()->getAnimation()->play("jump");
        role->getPhysicsBody()->jump();
        if (_velocityX != 0)
        {
            float direct = _velocityX > 0 ? 1 : -1;
            role->getPhysicsBody()->setVelocityX(speedX * direct);
        }
    }

    void JumpState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
    }

    void JumpState::execute(Role * role)
    {
        if (role->getPhysicsBody()->getY() == joker::PhysicsWorld::getInstance()->getGroundHeight())
        {
            float v = role->getPhysicsBody()->getVelocityX();
            if (abs(v) > 0)
                role->getStateManager()->changeState(SlowDownState::create(v));
            else
                role->getStateManager()->changeState(IdleState::create());
        }
    }

    void JumpState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN)
        {
            float direct = command.get<RoleDirection>("direction") == RoleDirection::LEFT ? -1 : 1;
            role->getPhysicsBody()->setVelocityX(speedX * direct);
            role->setDirection(command.get<RoleDirection>("direction"));
        }
        else if (roleAction == RoleAction::COLLIDE && role->isPlayer())
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
    }


    // CollideState
    CollideState::CollideState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void CollideState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("slowDown"));
        role->getArmature()->getAnimation()->play("slowDown");
        role->setDirection(_direction == RoleDirection::LEFT ? RoleDirection::RIGHT : RoleDirection::LEFT);
        float v = (_direction == RoleDirection::LEFT ? -1 : 1) * PhysicsBody::getDefaultSpeed();
        role->getPhysicsBody()->setVelocityX(v);
        role->getPhysicsBody()->setResistanceX(joker::PhysicsWorld::getInstance()->getResistance());
    }

    void CollideState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
    }

    void CollideState::execute(Role * role)
    {
        if (role->getPhysicsBody()->getVelocityX() == 0)
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    void CollideState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(PlayerAttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(PlayerAttackedState::create());
    }

}
