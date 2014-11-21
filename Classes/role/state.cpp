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
    }

    void StateManager::executeCommand(RoleAction command)
    {
        _currState->executeCommand(_role, command);
    }


    // IdleState
    void IdleState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("static"));
        role->getArmature()->getAnimation()->play("static");
    }

    void IdleState::executeCommand(Role * role, RoleAction command)
    {
        if (command == RoleAction::LEFT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::LEFT));
        else if (command == RoleAction::RIGHT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::RIGHT));
    }

    // RunState
    RunState::RunState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void RunState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("run"));
        role->getArmature()->getAnimation()->play("run");
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
            role->getStateManager()->changeState(SlowDownState::create(_direction));
    }

    // SlowDownState
    SlowDownState::SlowDownState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void SlowDownState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("slowDown"));
        role->getArmature()->getAnimation()->play("slowDown");
    }

    void SlowDownState::execute(Role * role)
    {
        // update physical body
    }

    void SlowDownState::executeCommand(Role * role, RoleAction command)
    {
        if (command == RoleAction::LEFT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::LEFT));
        else if (command == RoleAction::RIGHT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::RIGHT));
    }


}
