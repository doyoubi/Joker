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
        bool executeCommand(Role * role, const RoleCommand & command) override { return false; };
        std::string getDebugString() override;
    };

    class EnemyAttackedState : public State
    {
    public:
        static StatePtr create(RoleDirection direction) { return StatePtr(new EnemyAttackedState(direction)); }
        EnemyAttackedState(RoleDirection direction) : _direction(direction) {}
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override { return false; };
        std::string getDebugString() override;
    private:
        RoleDirection _direction;
    };

    class NodState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new NodState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override { return false; };
        std::string getDebugString() override;
    };

    class DefenceState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new DefenceState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override { return false; };
        std::string getDebugString() override;
    };

    class DefenceNodState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new DefenceNodState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override { return false; };
        std::string getDebugString() override;
    };

    class CrawlState : public State
    {
    public:
        static StatePtr create(RoleDirection direction) { return StatePtr(new CrawlState(direction)); }
        CrawlState(RoleDirection direction);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override {};
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    private:
        RoleDirection _direction;
    };

    class RoleActionNode;
    class FastRunState : public State
    {
    public:
        static StatePtr create(RoleDirection direction, RoleActionNode * btActionNode) { return StatePtr(new FastRunState(direction, btActionNode)); }
        FastRunState(RoleDirection direction, RoleActionNode * btActionNode);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override {};
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    private:
        RoleDirection _direction;
        RoleActionNode * _btActionNode;
    };

    class AttackReadyState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new AttackReadyState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override {};
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    };

    class RetreatNode;
    class RetreatState : public State
    {
    public:
        static StatePtr create(RoleDirection direction, RetreatNode * node) { return StatePtr(new RetreatState(direction, node)); }
        RetreatState(RoleDirection direction, RetreatNode * node) : _direction(direction), _retreatNode(node) {}
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    private:
        RoleDirection _direction;
        RetreatNode * _retreatNode;
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
