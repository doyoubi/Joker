#include <iostream>

#include "state.h"
#include "Role.h"
#include "utils/debug.h"
#include "SimplePhysics/PhysicsWorld.h"
#include "playerState.h"
#include "enemyState.h"

namespace joker
{
    // StateManager
    StateManager::StateManager(Role * role, StatePtr initState)
        : _role(role), _currState(std::move(initState))
    {
        CHECKNULL(_role);
        CHECKNULL(_currState);
        _currState->enterState(_role);
        Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    StateManager::~StateManager()
    {
        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    }

    void StateManager::changeState(StatePtr && nextState)
    {
        _currState->exitState(_role);
        _currState = std::move(nextState);
        _currState->enterState(_role);
    }

    void StateManager::update(float dt)
    {
        _currState->execute(_role);

        // update role with Physics position
        _role->setPosition(_role->getPhysicsBody()->getX(), _role->getPhysicsBody()->getY());
    }

    void StateManager::executeCommand(const RoleCommand & command)
    {
        _currState->executeCommand(_role, command);
    }

    void StateManager::printCurrState()
    {
        State * p = _currState.get();
        if (dynamic_cast<IdleState*>(p))
            cout << "idle state" << endl;
        else if (dynamic_cast<RunState*>(p))
            cout << "run state" << endl;
        else if (dynamic_cast<SlowDownState*>(p))
            cout << "slow down state" << endl;
        else if (dynamic_cast<PlayerAttackState*>(p))
            cout << "player attack state" << endl;
        else if (dynamic_cast<PlayerAttackedState*>(p))
            cout << "player attacked state" << endl;
        else if (dynamic_cast<EnemyAttackState*>(p))
            cout << "enemy attack state" << endl;
        else if (dynamic_cast<EnemyAttackedState*>(p))
            cout << "enemy attacked state" << endl;
        else if (dynamic_cast<JumpState*>(p))
            cout << "jump state" << endl;
        else if (dynamic_cast<NodState*>(p))
            cout << "nod state" << endl;
        else if (dynamic_cast<DefenceState*>(p))
            cout << "defence state" << endl;
        else if (dynamic_cast<DefenceNodState*>(p))
            cout << "defence nod state" << endl;
        else if (dynamic_cast<CrawlState*>(p))
            cout << "crawl state" << endl;
        else
            cout << "error: no match state" << endl;
    }


    // IdleState
    void IdleState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("static"));
        role->getArmature()->getAnimation()->play("static");
    }

    void IdleState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::FAST_RUN)
            role->getStateManager()->changeState(FastRunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::ATTACK && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackedState::create());
        else if (roleAction == RoleAction::ATTACK && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackedState::create());
        else if (roleAction == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create(0.0f));
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(NodState::create());
        else if (roleAction == RoleAction::DEFENCE)
            role->getStateManager()->changeState(DefenceState::create());
        else if (roleAction == RoleAction::COLLIDE && role->isPlayer())
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
    }

    // RunState
    RunState::RunState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void RunState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("run"));
        role->getArmature()->getAnimation()->play("run");
        role->setDirection(_direction);
        
        float speed = (_direction == RoleDirection::LEFT ? -1 : 1) * role->getNormalSpeed();

        role->getPhysicsBody()->setVelocityX(speed);
        role->getPhysicsBody()->setResistanceX(0);
    }

    void RunState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
    }

    void RunState::execute(Role * role)
    {
    }

    void RunState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN
            && _direction == command.get<RoleDirection>("direction"))
        {
            return;
        }

        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::FAST_RUN)
            role->getStateManager()->changeState(FastRunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::STOP && command.get<RoleDirection>("direction") == role->getDirection())
            role->getStateManager()->changeState(SlowDownState::create(
                    role->getPhysicsBody()->getVelocityX()
            ));
        else if (roleAction == RoleAction::ATTACK && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackedState::create());
        else if (roleAction == RoleAction::ATTACK && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackedState::create());
        else if (roleAction == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create(
            role->getPhysicsBody()->getVelocityX()
            ));
        else if (roleAction == RoleAction::DEFENCE)
            role->getStateManager()->changeState(CrawlState::create(
                role->getPhysicsBody()->getVelocityX() > 0 ? RoleDirection::RIGHT : RoleDirection::LEFT
            ));
        else if (roleAction == RoleAction::COLLIDE && role->isPlayer())
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
    }

    // SlowDownState
    SlowDownState::SlowDownState(float velocityX)
        : _velocityX(velocityX)
    {
    }

    void SlowDownState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("slowDown"));
        role->getArmature()->getAnimation()->play("slowDown");
        role->setDirection(_velocityX > 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        role->getPhysicsBody()->setVelocityX(_velocityX);
        role->getPhysicsBody()->setResistanceX(joker::PhysicsWorld::getInstance()->getResistance());
    }

    void SlowDownState::execute(Role * role)
    {
        if (role->getPhysicsBody()->getVelocityX() == 0)
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    void SlowDownState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
    }

    void SlowDownState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::FAST_RUN)
            role->getStateManager()->changeState(FastRunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::ATTACK && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && role->isPlayer())
            role->getStateManager()->changeState(PlayerAttackedState::create());
        else if (roleAction == RoleAction::ATTACK && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackState::create());
        else if (roleAction == RoleAction::ATTACKED && !role->isPlayer())
            role->getStateManager()->changeState(EnemyAttackedState::create());
        else if (roleAction == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create(
                role->getPhysicsBody()->getVelocityX()
            ));
        else if (roleAction == RoleAction::DEFENCE)
            role->getStateManager()->changeState(DefenceState::create());
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(NodState::create());
        else if (roleAction == RoleAction::COLLIDE && role->isPlayer())
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
    }

    
}
