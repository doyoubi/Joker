#ifndef JOKER_PLAYER_STATE
#define JOKER_PLAYER_STATE

#include "state.h"

namespace joker
{
    // this class is only used to share member '_currStage' between PlayerAttackState and EmptyAttackState
    class PlayerAttackBaseState : public State
    {
    protected:
        static const int attackStageQuantity;
        static int _currStage;
    };

    class PlayerAttackState : public PlayerAttackBaseState
    {
    public:
        static StatePtr create() { return StatePtr(new PlayerAttackState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    private:
        static const float changedDistance;
    };

    class EmptyAttackState : public PlayerAttackBaseState
    {
    public:
        static StatePtr create() { return StatePtr(new EmptyAttackState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    };

    class PlayerAttackedState : public State
    {
    public:
        static StatePtr create(RoleDirection direction) { return StatePtr(new PlayerAttackedState(direction)); }
        PlayerAttackedState(RoleDirection direction) : _direction(direction) {}
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    private:
        RoleDirection _direction;
    };

    class JumpState : public State
    {
    public:
        static StatePtr create(float velocityX) { return StatePtr(new JumpState(velocityX)); }
        JumpState(float velocityX);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    private:
        float _velocityX;
        static const float speedX;
    };

    class CollideState : public State
    {
    public:
        static StatePtr create(RoleDirection direction) { return StatePtr(new CollideState(direction)); }
        CollideState(RoleDirection direction);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        bool executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    private:
        RoleDirection _direction;
    };

}

#endif
