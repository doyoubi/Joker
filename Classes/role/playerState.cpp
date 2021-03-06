#include "cocostudio/CCArmature.h"

#include "playerState.h"
#include "Role.h"
#include "SimplePhysics/PhysicsWorld.h"
#include "utils/config.h"
#include "utils/debug.h"
#include "gameplay/BattleDirector.h"

namespace joker
{
    static std::string missingAnimation(const std::string animName)
    {
        return "player: missing '" + animName + "' animation";
    }

    // PlayerAttackBaseState
    int PlayerAttackBaseState::attackStageQuantity = 0;
    PlayerAttackBaseState::PlayerAttackBaseState()
    {
        // avoid Config used for static value initialization
        const static int q = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "attackStageQuantity" });
        attackStageQuantity = q;
    }
    int PlayerAttackBaseState::_currStage = 0;

    // PlayerAttackState
    std::string PlayerAttackState::getDebugString()
    {
        return "player attack";
    }

    void PlayerAttackState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "player", "PlayerAttackState" });
        static const float changedDistance = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "attackChangedDistance" });
        DEBUGCHECK(changedDistance >= 0, "changedDistance can't be negative");
        role->getRoleSprite()->playAnimAction(animName + std::to_string(_currStage));
        float d = role->getDirection() == RoleDirection::LEFT ? -changedDistance : changedDistance;
        role->getPhysicsBody()->setX(d + role->getPhysicsBody()->getX());

        role->getBattleDirector()->addEvent(EventPtr(new PlayerAttackEvent()));
    }

    void PlayerAttackState::execute(Role * role)
    {
        if (!role->getRoleSprite()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    void PlayerAttackState::exitState(Role * role)
    {
        _currStage = (_currStage + 1) % attackStageQuantity;
    }

    bool PlayerAttackState::executeCommand(Role * role, const RoleCommand & command)
    {
        return false;
    }

    // EmptyAttackState
    std::string EmptyAttackState::getDebugString()
    {
        return "empty attack";
    }

    void EmptyAttackState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "player", "PlayerAttackState" });
        role->getRoleSprite()->playAnimAction(animName + std::to_string(_currStage));

        role->getBattleDirector()->addEvent(EventPtr(new EmptyAttackEvent()));
    }

    void EmptyAttackState::exitState(Role * role)
    {
        _currStage = (_currStage + 1) % attackStageQuantity;
    }

    void EmptyAttackState::execute(Role * role)
    {
        if (!role->getRoleSprite()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    bool EmptyAttackState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(PlayerAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::COLLIDE)
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
        else return false;
        return true;
    }

    // PlayerAttackedState
    std::string PlayerAttackedState::getDebugString()
    {
        return "player attacked";
    }

    void PlayerAttackedState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "role", "attacked" });
        role->getRoleSprite()->playAnimAction(animName);

        static float attackedFallBackSpeed = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "attackedFallBackSpeed" });
        float v = (_direction == RoleDirection::LEFT ? 1 : -1) * attackedFallBackSpeed;
        role->getPhysicsBody()->setVelocityX(v);
        role->getPhysicsBody()->setResistanceX(joker::PhysicsWorld::getInstance()->getResistance());

        role->getBattleDirector()->getBattleJudge().reducePlayerHp();
    }

    void PlayerAttackedState::execute(Role * role)
    {
        if (!role->getRoleSprite()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    bool PlayerAttackedState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(PlayerAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::COLLIDE)
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
        else return false;
        return true;
    }


    // JumpState
    float JumpState::speedX = 0;

    std::string JumpState::getDebugString()
    {
        return "jump";
    }

    JumpState::JumpState(float velocityX)
        : _velocityX(velocityX)
    {
        // avoid Config used for static value initialization
        static const float s = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "jumpSpeedX" });
        JumpState::speedX = s;
    }

    void JumpState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "player", "JumpState" });
        role->getRoleSprite()->playAnimAction(animName);
        role->getPhysicsBody()->jump();
        if (_velocityX != 0)
        {
            float direct = _velocityX > 0 ? 1 : -1;
            role->getPhysicsBody()->setVelocityX(speedX * direct);
        }
    }

    void JumpState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
    }

    void JumpState::execute(Role * role)
    {
        if (role->getPhysicsBody()->getY() == joker::PhysicsWorld::getInstance()->getGroundHeight())
        {
            float v = role->getPhysicsBody()->getVelocityX();
            if (abs(v) > 0)
                role->getStateManager()->changeState(SlowDownState::create(v));
            else
                role->getStateManager()->changeState(IdleState::create());
        }
    }

    bool JumpState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN)
        {
            float direct = command.get<RoleDirection>("direction") == RoleDirection::LEFT ? -1 : 1;
            role->getPhysicsBody()->setVelocityX(speedX * direct);
            role->setDirection(command.get<RoleDirection>("direction"));
        }
        else if (roleAction == RoleAction::COLLIDE && role->isPlayer())
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(PlayerAttackedState::create(command.get<RoleDirection>("direction")));
        else return false;
        return true;
    }


    // CollideState
    std::string CollideState::getDebugString()
    {
        return "collide";
    }

    CollideState::CollideState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void CollideState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "player", "CollideState" });
        role->getRoleSprite()->playAnimAction(animName);
        role->setDirection(_direction == RoleDirection::LEFT ? RoleDirection::RIGHT : RoleDirection::LEFT);
        static float collideFallBackSpeed = Config::getInstance().getDoubleValue({"RoleProperty", "player", "collideFallBackSpeed"});
        float v = (_direction == RoleDirection::LEFT ? -1 : 1) * collideFallBackSpeed;
        role->getPhysicsBody()->setVelocityX(v);
        role->getPhysicsBody()->setResistanceX(joker::PhysicsWorld::getInstance()->getResistance());

        role->getBattleDirector()->getBattleJudge().reducePlayerHp();
    }

    void CollideState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
    }

    void CollideState::execute(Role * role)
    {
        if (role->getPhysicsBody()->getVelocityX() == 0)
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    bool CollideState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(PlayerAttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(PlayerAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::COLLIDE)
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
        else return false;
        return true;
    }

}
