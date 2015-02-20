#include <string>

#include "bombState.h"
#include "Role.h"
#include "utils/config.h"
#include "utils/debug.h"
#include "gameplay/Event.h"

namespace joker
{
    using std::string;

    static std::string missingAnimation(const std::string animName)
    {
        return "enemy: missing '" + animName + "' animation";
    }

    // FallingState
    std::string FallingState::getDebugString()
    {
        return "falling";
    }

    void FallingState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "bomb", "FallingState" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        role->getArmature()->getAnimation()->play(animName);
    }

    void FallingState::exitState(Role * role)
    {
    }

    void FallingState::execute(Role * role)
    {
    }

    bool FallingState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::EXPLODE)
            role->getStateManager()->changeState(ExplodeState::create());
        else return false;
        return true;
    }

    // ExplodeState
    std::string ExplodeState::getDebugString()
    {
        return "explode";
    }

    void ExplodeState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "bomb", "ExplodeState" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        role->getArmature()->getAnimation()->play(animName);
        role->getPhysicsBody()->setCollidable(true);
    }

    void ExplodeState::exitState(Role * role)
    {
    }

    void ExplodeState::execute(Role * role)
    {
        if (role->getArmature()->getAnimation()->isComplete())
        {
            role->die();
        }
    }

    bool ExplodeState::executeCommand(Role * role, const RoleCommand & command)
    {
        return false;
    }

}
