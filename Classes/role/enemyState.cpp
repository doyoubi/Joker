#include "enemyState.h"
#include "Role.h"
#include "utils/config.h"
#include "gameplay/BattleDirector.h"
#include "SimplePhysics/PhysicsWorld.h"

namespace joker
{
    static std::string missingAnimation(const std::string animName)
    {
        return "enemy: missing '" + animName + "' animation";
    }


    // EnemyAttackState
    std::string EnemyAttackState::getDebugString()
    {
        return "enemy attack";
    }

    void EnemyAttackState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "enemy", "EnemyAttackState" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        role->getArmature()->getAnimation()->play(animName);

        role->getBattleDirector()->addEvent(EventPtr(new EnemyAttackEvent()));
    }

    void EnemyAttackState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }


    // EnemyAttackedState
    std::string EnemyAttackedState::getDebugString()
    {
        return "enemy attacked";
    }

    void EnemyAttackedState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "role", "attacked" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        role->getArmature()->getAnimation()->play("attacked");

        static float attackedFallBackSpeed = Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "attackedFallBackSpeed" });
        float v = (_direction == RoleDirection::LEFT ? 1 : -1) * attackedFallBackSpeed;
        role->getPhysicsBody()->setVelocityX(v);
        role->getPhysicsBody()->setResistanceX(joker::PhysicsWorld::getInstance()->getResistance());
    }

    void EnemyAttackedState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->die();
        }
    }

    // NodState
    std::string NodState::getDebugString()
    {
        return "nod";
    }

    void NodState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "enemy", "NodState" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        role->getArmature()->getAnimation()->play(animName);
    }

    void NodState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    // DefenceState
    std::string DefenceState::getDebugString()
    {
        return "defence";
    }

    void DefenceState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "enemy", "DefenceState" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        role->getArmature()->getAnimation()->play(animName);

        static float defenceFallBackSpeed = Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "defenceFallBackSpeed" });
        float v = (role->getDirection() == RoleDirection::LEFT ? 1 : -1) * defenceFallBackSpeed;
        role->getPhysicsBody()->setVelocityX(v);
        role->getPhysicsBody()->setResistanceX(joker::PhysicsWorld::getInstance()->getResistance());
    }

    void DefenceState::execute(Role * role)
    {
        if (role->getArmature()->getAnimation()->isComplete())
            role->getStateManager()->changeState(IdleState::create());
    }

    void DefenceState::exitState(Role * role)
    {
    }


    // DefenceNodState
    std::string DefenceNodState::getDebugString()
    {
        return "defence nod";
    }

    void DefenceNodState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "enemy", "DefenceNodState" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        role->getArmature()->getAnimation()->play(animName);
    }

    void DefenceNodState::exitState(Role * role)
    {
    }

    void DefenceNodState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(DefenceState::create());
        }
    }

    // CrawlState
    std::string CrawlState::getDebugString()
    {
        return "crawl";
    }

    CrawlState::CrawlState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void CrawlState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "enemy", "CrawlState" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        role->getArmature()->getAnimation()->play(animName);

        float speed = (_direction == RoleDirection::LEFT ? -1 : 1) * role->getSlowSpeed();

        role->getPhysicsBody()->setVelocityX(speed);
        role->getPhysicsBody()->setResistanceX(0);
    }

    void CrawlState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
    }

    bool CrawlState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN
            && command.get<RoleDirection>("direction") == _direction)
        {
            return false;
        }

        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::FAST_RUN)
            role->getStateManager()->changeState(FastRunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::STOP && command.get<RoleDirection>("direction") == role->getDirection())
            role->getStateManager()->changeState(DefenceState::create());
        else if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(EnemyAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(DefenceNodState::create());
        else if (roleAction == RoleAction::IDLE)
            role->getStateManager()->changeState(IdleState::create());
        else if (roleAction == RoleAction::ATTACK_READY)
            role->getStateManager()->changeState(AttackReadyState::create());
        else return false;
        return true;
    }

    // FastRunState
    std::string FastRunState::getDebugString()
    {
        return "fast run";
    }

    FastRunState::FastRunState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void FastRunState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "enemy", "FastRunState" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        const static float fastSpeed = Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "fastSpeed" });
        role->getArmature()->getAnimation()->play(animName);
        float speed = (_direction == RoleDirection::LEFT ? -1 : 1) * fastSpeed;

        role->getPhysicsBody()->setVelocityX(speed);
        role->getPhysicsBody()->setResistanceX(0);
    }

    void FastRunState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
    }

    bool FastRunState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN
            && command.get<RoleDirection>("direction") == _direction)
        {
            return false;
        }

        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::FAST_RUN)
            role->getStateManager()->changeState(FastRunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::STOP && command.get<RoleDirection>("direction") == role->getDirection())
            role->getStateManager()->changeState(SlowDownState::create(role->getPhysicsBody()->getVelocityX()));
        else if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(EnemyAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::IDLE)
            role->getStateManager()->changeState(IdleState::create());
        else if (roleAction == RoleAction::DEFENCE)
            role->getStateManager()->changeState(DefenceState::create());
        else return false;
        return true;
    }

    // AttackReadyState
    std::string AttackReadyState::getDebugString()
    {
        return "attack ready";
    }

    void AttackReadyState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "enemy", "AttackReadyState" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        role->getArmature()->getAnimation()->play(animName);
    }

    bool AttackReadyState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(EnemyAttackedState::create(command.get<RoleDirection>("direction")));
        else return false;
        return true;
    }

    // RetreatState
    std::string RetreatState::getDebugString()
    {
        return "retreat";
    }

    void RetreatState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "enemy", "RetreatState" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        static const float retreastSpeed = Config::getInstance().getDoubleValue({"RoleProperty", "enemy", "restreatSpeed"});
        role->getArmature()->getAnimation()->play(animName);
        float speed = (_direction == RoleDirection::LEFT ? 1 : -1) * retreastSpeed;

        role->getPhysicsBody()->setVelocityX(speed);
        role->getPhysicsBody()->setResistanceX(joker::PhysicsWorld::getInstance()->getResistance());
    }

    void RetreatState::execute(Role * role)
    {
        if (role->getPhysicsBody()->getVelocityX() == 0.0f)
            role->getStateManager()->changeState(IdleState::create());
    }

    void RetreatState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
        _retreatNode->setExit(true);
    }

    bool RetreatState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN
            && command.get<RoleDirection>("direction") == _direction)
        {
            return false;
        }

        if (roleAction == RoleAction::STOP && command.get<RoleDirection>("direction") == role->getDirection())
            role->getStateManager()->changeState(SlowDownState::create(role->getPhysicsBody()->getVelocityX()));
        else if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(EnemyAttackedState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::IDLE)
            role->getStateManager()->changeState(IdleState::create());
        else if (roleAction == RoleAction::DEFENCE)
            role->getStateManager()->changeState(DefenceState::create());
        else return false;
        return true;
    }

    // EnterState
    std::string EnterState::getDebugString()
    {
        return "enter state";
    }
    void EnterState::enterState(Role * role)
    {
        static const string animName = Config::getInstance().getStringValue({ "animation", "enemy", "EnterState" });
        DEBUGCHECK(role->getArmature()->getAnimation()->getAnimationData()->getMovement(animName) != nullptr,
            missingAnimation(animName));
        role->getArmature()->getAnimation()->play(animName);
    }
    void EnterState::execute(Role * role)
    {
        if (role->getArmature()->getAnimation()->isComplete())
            role->getStateManager()->changeState(IdleState::create());
    }

}
