#include <iostream>

#include "state.h"
#include "Role.h"
#include "utils/debug.h"
#include "SimplePhysics/PhysicsWorld.h"
#include "playerState.h"
#include "enemyState.h"
#include "utils/config.h"
#include "bombState.h"

namespace joker
{
    // StateManager
    StateManager::StateManager(Role * role, StatePtr initState)
        : _role(role), _currState(std::move(initState))
    {
        CHECKNULL(_role);
        CHECKNULL(_currState);
        _currState->enterState(_role);
        Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    StateManager::~StateManager()
    {
        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    }

    void StateManager::changeState(StatePtr && nextState)
    {
        _currState->exitState(_role);
        _currState = std::move(nextState);
        _currState->enterState(_role);
    }

    void StateManager::update(float dt)
    {
        _currState->execute(_role);

        // update role with Physics position
        _role->setPosition(_role->getPhysicsBody()->getX(), _role->getPhysicsBody()->getY());
    }

    bool StateManager::executeCommand(const RoleCommand & command)
    {
        return _currState->executeCommand(_role, command);
    }

    std::string StateManager::getDebugString()
    {
        return _currState->getDebugString();
    }


    static std::string missingAnimation(Role * role, const std::string animName)
    {
        return std::string(role->isPlayer() ? "player" : "enemy") + ": missing '" + animName + "' animation";
    }


    // IdleState
    std::string IdleState::getDebugString()
    {
        return "Idle";
    }

    void IdleState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({"animation", "role", "IdleState"});
        role->getRoleSprite()->playAnimAction(animName);
    }

    bool IdleState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::FAST_RUN)
            role->getStateManager()->changeState(FastRunState::create(
            command.get<RoleDirection>("direction"), command.get<RoleActionNode*>("btActionNode")));
        else if (roleAction == RoleAction::ATTACK && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::ATTACK && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create(0.0f));
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(NodState::create());
        else if (roleAction == RoleAction::DEFENCE)
            role->getStateManager()->changeState(DefenceState::create());
        else if (roleAction == RoleAction::COLLIDE && role->isPlayer())
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::ATTACK_READY)
            role->getStateManager()->changeState(AttackReadyState::create());
        else if (roleAction == RoleAction::FALLING)
            role->getStateManager()->changeState(FallingState::create());
        else if (roleAction == RoleAction::EMPTY_ATTACK)
            role->getStateManager()->changeState(EmptyAttackState::create());
        else if (roleAction == RoleAction::RETREAT)
            role->getStateManager()->changeState(RetreatState::create(role->getDirection(), command.get<RetreatNode*>("btnode")));
        else return false;
        return true;
    }

    // RunState
    std::string RunState::getDebugString()
    {
        return "run";
    }

    RunState::RunState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void RunState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "role", "RunState" });
        role->getRoleSprite()->playAnimAction(animName);
        role->setDirection(_direction);
        
        float speed = (_direction == RoleDirection::LEFT ? -1 : 1) * role->getNormalSpeed();

        role->getPhysicsBody()->setVelocityX(speed);
        role->getPhysicsBody()->setResistanceX(0);
    }

    void RunState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
    }

    void RunState::execute(Role * role)
    {
    }

    bool RunState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN
            && _direction == command.get<RoleDirection>("direction"))
        {
            return false;
        }

        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::FAST_RUN)
            role->getStateManager()->changeState(FastRunState::create(
            command.get<RoleDirection>("direction"), command.get<RoleActionNode*>("btActionNode")));
        else if (roleAction == RoleAction::STOP && command.get<RoleDirection>("direction") == role->getDirection())
            role->getStateManager()->changeState(SlowDownState::create(
                    role->getPhysicsBody()->getVelocityX()
            ));
        else if (roleAction == RoleAction::ATTACK && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::ATTACK && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create(
            role->getPhysicsBody()->getVelocityX()
            ));
        else if (roleAction == RoleAction::DEFENCE)
            role->getStateManager()->changeState(DefenceState::create());
        else if (roleAction == RoleAction::COLLIDE && role->isPlayer())
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::EMPTY_ATTACK)
            role->getStateManager()->changeState(EmptyAttackState::create());
        else return false;
        return true;
    }

    // SlowDownState
    std::string SlowDownState::getDebugString()
    {
        return "slow down";
    }

    SlowDownState::SlowDownState(float velocityX)
        : _velocityX(velocityX)
    {
    }

    void SlowDownState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "role", "SlowDownState" });
        role->getRoleSprite()->playAnimAction(animName);
        role->setDirection(_velocityX > 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        role->getPhysicsBody()->setVelocityX(_velocityX);
        role->getPhysicsBody()->setResistanceX(joker::PhysicsWorld::getInstance()->getResistance());
    }

    void SlowDownState::execute(Role * role)
    {
        if (role->getPhysicsBody()->getVelocityX() == 0)
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    void SlowDownState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
    }

    bool SlowDownState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::FAST_RUN)
            role->getStateManager()->changeState(FastRunState::create(
            command.get<RoleDirection>("direction"), command.get<RoleActionNode*>("btActionNode")));
        else if (roleAction == RoleAction::ATTACK && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::ATTACK && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create(
                role->getPhysicsBody()->getVelocityX()
            ));
        else if (roleAction == RoleAction::DEFENCE)
            role->getStateManager()->changeState(DefenceState::create());
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(NodState::create());
        else if (roleAction == RoleAction::COLLIDE && role->isPlayer())
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::EMPTY_ATTACK)
            role->getStateManager()->changeState(EmptyAttackState::create());
        else return false;
        return true;
    }

    // EnterState
    std::string EnterState::getDebugString()
    {
        return "enter state";
    }

    void EnterState::enterState(Role * role)
    {
        const char * roleTypeName = role->isPlayer() ? "player" : "enemy";
        static const string animName = Config::getInstance().getStringValue({ "animation", roleTypeName, "EnterState" });
        role->getRoleSprite()->playAnimAction(animName);
    }

    void EnterState::execute(Role * role)
    {
        if (role->getRoleSprite()->isComplete())
            role->getStateManager()->changeState(IdleState::create());
    }

    
}
