#ifndef JOKER_STATE
#define JOKER_STATE

#include <memory>

#include "cocos2d.h"

#include "RoleEnumType.h"
#include "SimplePhysics/PhysicsBody.h"

namespace joker
{

    using std::unique_ptr;
    class State;
    typedef unique_ptr<State> StatePtr;

    class Role;

    class StateManager
    {
    public:
        // require armature of role finish initialization
        StateManager(Role * role, StatePtr initState);
        ~StateManager();
        void changeState(StatePtr && nextState);
        void executeCommand(RoleAction command);
        void update(float dt);

        void printCurrState();   // for debug
    private:
        Role * _role;   // weak reference
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
        virtual void executeCommand(Role * role, RoleAction command) = 0;
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
        void executeCommand(Role * role, RoleAction command) override;
    };

    class RunState : public State
    {
    public:
        static StatePtr create(RoleDirection direction) { return StatePtr(new RunState(direction)); }
        RunState(RoleDirection direction);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        void executeCommand(Role * role, RoleAction command) override;
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
        void executeCommand(Role * role, RoleAction command) override;
    private:
        float _velocityX;
    };

    class AttackState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new AttackState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override;
        void executeCommand(Role * role, RoleAction command) override {};
    };

    class AttackedState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new AttackedState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override;
        void executeCommand(Role * role, RoleAction command) override {};
    };

    class JumpState : public State
    {
    public:
        static StatePtr create(float velocityX) { return StatePtr(new JumpState(velocityX)); }
        JumpState(float velocityX);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        void executeCommand(Role * role, RoleAction command) override {};
    private:
        float _velocityX;
    };

    class NodState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new NodState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override;
        void executeCommand(Role * role, RoleAction command) override {};
    };

    // for enemy
    class DefenceState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new DefenceState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override {};
        void executeCommand(Role * role, RoleAction command) override;
    };

    // for enemy
    class DefenceNodState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new DefenceNodState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        void executeCommand(Role * role, RoleAction command) override {};
    };

    // for enemy
    class CrawlState : public State
    {
    public:
        static StatePtr create(RoleDirection direction) { return StatePtr(new CrawlState(direction)); }
        CrawlState(RoleDirection direction);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override {};
        void executeCommand(Role * role, RoleAction command) override;
    private:
        RoleDirection _direction;
    };

    class CollideState : public State
    {
    public:
        static StatePtr create(RoleAction collide_direction) { return StatePtr(new CollideState(collide_direction)); }
        CollideState(RoleAction collide_direction);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        void executeCommand(Role * role, RoleAction command) override;
    private:
        RoleAction _collide_direction;
    };

}

#endif
