#include "state.h"
#include "utils/debug.h"

namespace joker
{
    // StateManager
    StateManager::StateManager(Role * role, StatePtr initState)
        : _role(role), currState(std::move(initState))
    {
        CHECKNULL(_role);
        CHECKNULL(currState);
        Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    StateManager::~StateManager()
    {
        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    }

    void StateManager::changeState(StatePtr && nextState)
    {
        currState->exitState(_role);
        currState = std::move(nextState);
        currState->enterState(_role);
    }

    void StateManager::update(float dt)
    {
        currState->execute(getRole());
    }

    void StateManager::executeCommand(RoleAction command)
    {
        currState->executeCommand(_role, command);
    }


}
