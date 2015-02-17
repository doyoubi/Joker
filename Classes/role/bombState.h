#ifndef JOKER_BOMB_STATE
#define JOKER_BOMB_STATE

#include "state.h"

namespace joker
{
    class FallingState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new FallingState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        void executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    };

    class ExplodeState : public State
    {
    public:
        static StatePtr create() { return StatePtr(new ExplodeState()); }
        void enterState(Role * role) override;
        void exitState(Role * role) override;
        void execute(Role * role) override;
        void executeCommand(Role * role, const RoleCommand & command) override;
        std::string getDebugString() override;
    };
}

#endif
