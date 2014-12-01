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
        else if (command == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create(false));
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
        else if (command == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create(true));
    }

    // SlowDownState
    SlowDownState::SlowDownState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void SlowDownState::enterState(Role * role)
    {
        cout << "enter slow down state" << endl;
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("slowDown"));
        role->getArmature()->getAnimation()->play("slowDown");
        role->setDirection(_direction);
        // let player slow down
        role->getSimplePhysics()->setResistanceX(SimplePhysics::getResistance());
    }

    void SlowDownState::execute(Role * role)
    {
        // update physical body
        if (!role->getArmature()->getAnimation()->isPlaying()
            && role->getSimplePhysics()->getVelocityX() == 0)
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    void SlowDownState::executeCommand(Role * role, RoleAction command)
    {
        if (command == RoleAction::LEFT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::LEFT));
        else if (command == RoleAction::RIGHT_RUN)
            role->getStateManager()->changeState(RunState::create(RoleDirection::RIGHT));
    }

    // JumpState
    JumpState::JumpState(bool running):
        _running(running)
    {
    }

    void JumpState::enterState(Role * role)
    {
        cout << "enter jump state" << endl;
        role->getSimplePhysics()->setVelocityY(300);
    }

    void JumpState::execute(Role * role)
    {
        // update physical body
        if (role->getSimplePhysics()->getY() == SimplePhysics::getGroundHeight())
        {
            if (_running)
                role->getStateManager()->changeState(SlowDownState::create(role->getDirection()));
            else
                role->getStateManager()->changeState(IdleState::create());
        }
    }

    void JumpState::executeCommand(Role * role, RoleAction command)
    {
    }
}
