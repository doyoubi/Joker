#ifndef JOKER_ENEMY_STATE
#define JOKER_ENEMY_STATE

#include "state.h"

namespace joker
{
    class EnemyAttackState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new EnemyAttackState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override;
        void executeCommand(Role * role, const RoleCommand & command) override {};
    };

    class EnemyAttackedState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new EnemyAttackedState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override;
        void executeCommand(Role * role, const RoleCommand & command) override {};
    };

    class NodState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new NodState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override;
        void executeCommand(Role * role, const RoleCommand & command) override {};
    };

    class DefenceState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new DefenceState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override {};
        void executeCommand(Role * role, const RoleCommand & command) override;
    };

    class DefenceNodState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new DefenceNodState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        void executeCommand(Role * role, const RoleCommand & command) override {};
    };

    class CrawlState : public State
    {
    public:
        static StatePtr create(RoleDirection direction) { return StatePtr(new CrawlState(direction)); }
        CrawlState(RoleDirection direction);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override {};
        void executeCommand(Role * role, const RoleCommand & command) override;
    private:
        RoleDirection _direction;
    };

    class FastRunState : public State
    {
    public:
        static StatePtr create(RoleDirection direction) { return StatePtr(new FastRunState(direction)); }
        FastRunState(RoleDirection direction);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override {};
        void executeCommand(Role * role, const RoleCommand & command) override;
    private:
        RoleDirection _direction;
    };

}

#endif
