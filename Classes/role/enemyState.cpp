#include "enemyState.h"
#include "Role.h"

namespace joker
{
    // EnemyAttackState
    void EnemyAttackState::enterState(Role * role)
    {
        role->getArmature()->getAnimation()->play("attack");
        const float changedDistance = 200;
        float d = role->getDirection() == RoleDirection::LEFT ? -changedDistance : changedDistance;
    }

    void EnemyAttackState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }


    // EnemyAttackedState
    void EnemyAttackedState::enterState(Role * role)
    {
        role->getArmature()->getAnimation()->play("attacked");
    }

    void EnemyAttackedState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    // NodState
    void NodState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("nod"));
        role->getArmature()->getAnimation()->play("nod");
    }

    void NodState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    // DefenceState
    void DefenceState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("defence"));
        role->getArmature()->getAnimation()->play("defence");
        role->getPhysicsBody()->setCollidable(true);
    }

    void DefenceState::exitState(Role * role)
    {
        role->getPhysicsBody()->setCollidable(false);
    }

    void DefenceState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::FAST_RUN)
            role->getStateManager()->changeState(FastRunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(EnemyAttackedState::create());
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(DefenceNodState::create());
        else if (roleAction == RoleAction::IDLE)
            role->getStateManager()->changeState(IdleState::create());
    }

    // DefenceNodState
    void DefenceNodState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("defenceNod"));
        role->getArmature()->getAnimation()->play("defenceNod");
        role->getPhysicsBody()->setCollidable(true);
    }

    void DefenceNodState::exitState(Role * role)
    {
        role->getPhysicsBody()->setCollidable(false);
    }

    void DefenceNodState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(DefenceState::create());
        }
    }

    // CrawlState
    CrawlState::CrawlState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void CrawlState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("defence"));
        role->getArmature()->getAnimation()->play("defence");

        float speed = (_direction == RoleDirection::LEFT ? -1 : 1) * role->getSlowSpeed();

        role->getPhysicsBody()->setVelocityX(speed);
        role->getPhysicsBody()->setResistanceX(0);
        role->getPhysicsBody()->setCollidable(true);
    }

    void CrawlState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
        role->getPhysicsBody()->setCollidable(false);
    }

    void CrawlState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN
            && command.get<RoleDirection>("direction") == _direction)
        {
            return;
        }

        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::FAST_RUN)
            role->getStateManager()->changeState(FastRunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::STOP)
            role->getStateManager()->changeState(DefenceState::create());
        else if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(EnemyAttackedState::create());
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(DefenceNodState::create());
        else if (roleAction == RoleAction::IDLE)
            role->getStateManager()->changeState(IdleState::create());
    }

    // FastRunState
    FastRunState::FastRunState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void FastRunState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("fastRun"));
        role->getArmature()->getAnimation()->play("fastRun");

        const float fastSpeed = role->getNormalSpeed() * 3;
        float speed = (_direction == RoleDirection::LEFT ? -1 : 1) * fastSpeed;

        role->getPhysicsBody()->setVelocityX(speed);
        role->getPhysicsBody()->setResistanceX(0);
        role->getPhysicsBody()->setCollidable(false);
    }

    void FastRunState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
    }

    void FastRunState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN
            && command.get<RoleDirection>("direction") == _direction)
        {
            return;
        }

        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::FAST_RUN)
            role->getStateManager()->changeState(FastRunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::STOP)
            role->getStateManager()->changeState(DefenceState::create());
        else if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(EnemyAttackedState::create());
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(DefenceNodState::create());
        else if (roleAction == RoleAction::IDLE)
            role->getStateManager()->changeState(IdleState::create());
    }

}
