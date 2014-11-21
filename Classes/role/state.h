#ifndef JOKER_STATE
#define JOKER_STATE

#include <memory>

#include "cocos2d.h"

namespace joker
{

    using std::unique_ptr;
    class State;
    typedef unique_ptr<State> StatePtr;

    class Role;

    class StateManager
    {
    public:
        StateManager(Role * role, StatePtr initState);
        ~StateManager();
        void changeState(StatePtr && nextState);
        Role * getRole() { return _role; }
        void update(float dt);
    private:
        Role * _role;   // weak reference
        StatePtr currState;

        CC_DISALLOW_COPY_AND_ASSIGN(StateManager);
    };

    class State
    {
    public:
        State() = default;
        virtual void enterState(Role * role) = 0;
        virtual void exitState(Role * role) = 0;
        virtual void execute(Role * role) = 0;
    private:
        CC_DISALLOW_COPY_AND_ASSIGN(State);
    };

    class IdleState
    {
    public:
        virtual void enterState(Role * role) {}
        virtual void exitState(Role * role) {};
        virtual void execute(Role * role) {};
    };

}

#endif
