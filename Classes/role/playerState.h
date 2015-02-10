#ifndef JOKER_PLAYER_STATE
#define JOKER_PLAYER_STATE

#include "state.h"

namespace joker
{

    class PlayerAttackState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new PlayerAttackState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        void executeCommand(Role * role, const RoleCommand & command) override {};
    private:
        static int _currStage;
        static const int attackStageQuantity;
        static const float changedDistance;
    };

    class PlayerAttackedState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new PlayerAttackedState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override {};
        void execute(Role * role) override;
        void executeCommand(Role * role, const RoleCommand & command) override {};
    };

    class JumpState : public State
    {
    public:
        static StatePtr create(float velocityX) { return StatePtr(new JumpState(velocityX)); }
        JumpState(float velocityX);
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        void executeCommand(Role * role, const RoleCommand & command) override;
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
        void executeCommand(Role * role, const RoleCommand & command) override;
    private:
        RoleDirection _direction;
    };

}

#endif
