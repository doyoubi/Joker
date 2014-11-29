#include <iostream>

#include "state.h"
#include "Role.h"
#include "utils/debug.h"

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
        _role->setPosition(_role->getSimplePhysics()->getX(), _role->getSimplePhysics()->getY());
    }

    void StateManager::executeCommand(RoleAction command)
    {
        _currState->executeCommand(_role, command);
    }


    // IdleState
    void IdleState::enterState(Role * role)
    {
        cout << "enter idle state" << endl;
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("static"));
        role->getArmature()->getAnimation()->play("static");
    }

    void IdleState::executeCommand(Role * role, RoleAction command)
    {
        if (command == RoleAction::LEFT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::LEFT));
        else if (command == RoleAction::RIGHT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::RIGHT));
        else if (command == RoleAction::ATTACK)
            role->getStateManager()->changeState(AttackState::create());
        else if (command == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create());
    }

    // RunState
    RunState::RunState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void RunState::enterState(Role * role)
    {
        cout << "enter run state" << endl;
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("run"));
        role->getArmature()->getAnimation()->play("run");
        role->setDirection(_direction);
        
        float speed = (_direction == RoleDirection::LEFT ? -1 : 1) * SimplePhysics::getDefaultSpeed();

        role->getSimplePhysics()->setVelocityX(speed);
        role->getSimplePhysics()->setResistanceX(0);
    }

    void RunState::exitState(Role * role)
    {
        role->getSimplePhysics()->setVelocityX(0);
        role->getSimplePhysics()->setResistanceX(0);
    }

    void RunState::execute(Role * role)
    {
        // update physical body
    }

    void RunState::executeCommand(Role * role, RoleAction command)
    {
        if (_direction == RoleDirection::LEFT && command == RoleAction::LEFT_RUN) return;
        if (_direction == RoleDirection::RIGHT && command == RoleAction::RIGHT_RUN) return;

        if (command == RoleAction::LEFT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::LEFT));
        else if (command == RoleAction::RIGHT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::RIGHT));
        else if (command == RoleAction::STOP)
            role->getStateManager()->changeState(SlowDownState::create(
                    role->getSimplePhysics()->getVelocityX()
            ));
        else if (command == RoleAction::ATTACK)
            role->getStateManager()->changeState(AttackState::create());
        else if (command == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create());
    }

    // SlowDownState
    SlowDownState::SlowDownState(float velocityX)
        : _velocityX(velocityX)
    {
    }

    void SlowDownState::enterState(Role * role)
    {
        cout << "enter slow down state" << endl;
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("slowDown"));
        role->getArmature()->getAnimation()->play("slowDown");
        role->setDirection(_velocityX > 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        role->getSimplePhysics()->setVelocityX(_velocityX);
        role->getSimplePhysics()->setResistanceX(SimplePhysics::getResistance());
    }

    void SlowDownState::execute(Role * role)
    {
        if (role->getSimplePhysics()->getVelocityX() == 0)
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    void SlowDownState::exitState(Role * role)
    {
        role->getSimplePhysics()->setVelocityX(0);
        role->getSimplePhysics()->setResistanceX(0);
    }

    void SlowDownState::executeCommand(Role * role, RoleAction command)
    {
        if (command == RoleAction::LEFT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::LEFT));
        else if (command == RoleAction::RIGHT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::RIGHT));
        else if (command == RoleAction::ATTACK)
            role->getStateManager()->changeState(AttackState::create());
        else if (command == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create());
    }

    // AttackState
    void AttackState::enterState(Role * role)
    {
        cout << "enter attack state" << endl;
        role->getArmature()->getAnimation()->play("attack");
    }

    void AttackState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    // AttackedState
    void AttackedState::enterState(Role * role)
    {
        cout << "enter attacked state" << endl;
        role->getArmature()->getAnimation()->play("attacked");
    }

    void AttackedState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    // JumpState
    void JumpState::enterState(Role * role)
    {
        cout << "enter jump state" << endl;
        role->getArmature()->getAnimation()->play("jump");
        role->getSimplePhysics()->jump();
    }

    void JumpState::execute(Role * role)
    {
        if (!role->getSimplePhysics()->isJumping())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }


}
