#ifndef JOKER_STATE
#define JOKER_STATE

#include <memory>
#include <string>

#include "cocos2d.h"

#include "RoleEnumType.h"
#include "SimplePhysics/PhysicsBody.h"

namespace joker
{

    using std::unique_ptr;
    class State;
    typedef unique_ptr<State> StatePtr;

    class Role;
    struct RoleCommand;

    class StateManager
    {
    public:
        // require armature of role finish initialization
        StateManager(Role * role, StatePtr initState);
        ~StateManager();
        void changeState(StatePtr && nextState);
        bool executeCommand(const RoleCommand & command);
        void update(float dt);

        std::string getDebugString();
    private:
        Role * _role;   // weak referencebool executeCommand
        StatePtr _currState;

        CC_DISALLOW_COPY_AND_ASSIGN(StateManager);
    };

    class State
    {
    public:
        State() = default;
        virtual void enterState(Role * role) = 0;
        virtual void exitState(Role * role) = 0;
        virtual void execute(Role * role) = 0;
        virtual bool executeCommand(Role * role, const RoleCommand & command) = 0;

        virtual std::string getDebugString() = 0;
    private:
        CC_DISALLOW_COPY_AND_ASSIGN(State);
    };

    class IdleState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new IdleState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override {};
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    };

    class RunState : public State
    {
    public:
        static StatePtr create(RoleDirection direction) { return StatePtr(new RunState(direction)); }
        RunState(RoleDirection direction);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    private:
        RoleDirection _direction;
    };

    class SlowDownState : public State
    {
    public:
        static StatePtr create(float velocityX) { return StatePtr(new SlowDownState(velocityX)); }
        SlowDownState(float velocityX);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    private:
        float _velocityX;
    };

    class EnterState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new EnterState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override { return false; };
        std::string getDebugString() override;
    };

}

#endif
