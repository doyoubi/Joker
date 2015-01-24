#include <iostream>

#include "state.h"
#include "Role.h"
#include "utils/debug.h"
#include "SimplePhysics/PhysicsWorld.h"

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
        else if (dynamic_cast<AttackState*>(p))
            cout << "attack state" << endl;
        else if (dynamic_cast<AttackedState*>(p))
            cout << "attacked state" << endl;
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
        else if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(AttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(AttackedState::create());
        else if (roleAction == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create(0.0f));
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(NodState::create());
        else if (roleAction == RoleAction::DEFENCE)
            role->getStateManager()->changeState(DefenceState::create());
        else if (roleAction == RoleAction::COLLIDE)
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
        else if (roleAction == RoleAction::STOP)
            role->getStateManager()->changeState(SlowDownState::create(
                    role->getPhysicsBody()->getVelocityX()
            ));
        else if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(AttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(AttackedState::create());
        else if (roleAction == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create(
            role->getPhysicsBody()->getVelocityX()
            ));
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(NodState::create());
        else if (roleAction == RoleAction::DEFENCE)
            role->getStateManager()->changeState(CrawlState::create(
                role->getPhysicsBody()->getVelocityX() > 0 ? RoleDirection::RIGHT : RoleDirection::LEFT
            ));
        else if (roleAction == RoleAction::COLLIDE)
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
        else if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(AttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(AttackedState::create());
        else if (roleAction == RoleAction::JUMP)
            role->getStateManager()->changeState(JumpState::create(
                role->getPhysicsBody()->getVelocityX()
            ));
        else if (roleAction == RoleAction::DEFENCE)
            role->getStateManager()->changeState(DefenceState::create());
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(NodState::create());
        else if (roleAction == RoleAction::COLLIDE)
            role->getStateManager()->changeState(CollideState::create(command.get<RoleDirection>("direction")));
    }

    // AttackState
    void AttackState::enterState(Role * role)
    {
        role->getArmature()->getAnimation()->play("attack");
        const float changedDistance = 200;
        float d = role->getDirection() == RoleDirection::LEFT ? -changedDistance : changedDistance;
        role->getPhysicsBody()->setX(d + role->getPhysicsBody()->getX());
    }

    void AttackState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    // AttackedState
    void AttackedState::enterState(Role * role)
    {
        role->getArmature()->getAnimation()->play("attacked");
    }

    void AttackedState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    // JumpState
    JumpState::JumpState(float velocityX)
        : _velocityX(velocityX)
    {
    }

    void JumpState::enterState(Role * role)
    {
        role->getArmature()->getAnimation()->play("jump");
        role->getPhysicsBody()->jump();
    }

    void JumpState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
    }

    void JumpState::execute(Role * role)
    {
        if (role->getPhysicsBody()->getY() == joker::PhysicsWorld::getInstance()->getGroundHeight())
        {
            if (abs(_velocityX) > 0)
                role->getStateManager()->changeState(SlowDownState::create(_velocityX));
            else
                role->getStateManager()->changeState(IdleState::create());
        }
    }

    // NodState
    void NodState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("nod"));
        role->getArmature()->getAnimation()->play("nod");
    }

    void NodState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(IdleState::create());
        }
    }

    // DefenceState
    void DefenceState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("defence"));
        role->getArmature()->getAnimation()->play("defence");
        role->getPhysicsBody()->setCollidable(true);
    }

    void DefenceState::exitState(Role * role)
    {
        role->getPhysicsBody()->setCollidable(false);
    }

    void DefenceState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(AttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(AttackedState::create());
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(DefenceNodState::create());
        else if (roleAction == RoleAction::IDLE)
            role->getStateManager()->changeState(IdleState::create());
    }

    // DefenceNodState
    void DefenceNodState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("defenceNod"));
        role->getArmature()->getAnimation()->play("defenceNod");
        role->getPhysicsBody()->setCollidable(true);
    }

    void DefenceNodState::exitState(Role * role)
    {
        role->getPhysicsBody()->setCollidable(false);
    }

    void DefenceNodState::execute(Role * role)
    {
        if (!role->getArmature()->getAnimation()->isPlaying())
        {
            role->getStateManager()->changeState(DefenceState::create());
        }
    }

    // CrawlState
    CrawlState::CrawlState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void CrawlState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("defence"));
        role->getArmature()->getAnimation()->play("defence");

        float speed = (_direction == RoleDirection::LEFT ? -1 : 1) * role->getSlowSpeed();

        role->getPhysicsBody()->setVelocityX(speed);
        role->getPhysicsBody()->setResistanceX(0);
        role->getPhysicsBody()->setCollidable(true);
    }

    void CrawlState::exitState(Role * role)
    {
        role->getPhysicsBody()->setVelocityX(0);
        role->getPhysicsBody()->setResistanceX(0);
        role->getPhysicsBody()->setCollidable(false);
    }

    void CrawlState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::RUN
            && command.get<RoleDirection>("direction") == _direction)
        {
            return;
        }

        if (roleAction == RoleAction::RUN)
            role->getStateManager()->changeState(RunState::create(command.get<RoleDirection>("direction")));
        else if (roleAction == RoleAction::STOP)
            role->getStateManager()->changeState(DefenceState::create());
        else if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(AttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(AttackedState::create());
        else if (roleAction == RoleAction::NOD)
            role->getStateManager()->changeState(DefenceNodState::create());
        else if (roleAction == RoleAction::IDLE)
            role->getStateManager()->changeState(IdleState::create());
    }


    // CollideState
    CollideState::CollideState(RoleDirection direction)
        : _direction(direction)
    {
    }

    void CollideState::enterState(Role * role)
    {
        CHECKNULL(role->getArmature()->getAnimation()->getAnimationData()->getMovement("slowDown"));
        role->getArmature()->getAnimation()->play("slowDown");
        role->setDirection(_direction == RoleDirection::LEFT ? RoleDirection::RIGHT : RoleDirection::LEFT);
        float v = (_direction == RoleDirection::LEFT ? -1 : 1) * PhysicsBody::getDefaultSpeed();
        role->getPhysicsBody()->setVelocityX(v);
        role->getPhysicsBody()->setResistanceX(joker::PhysicsWorld::getInstance()->getResistance());
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

    void CollideState::executeCommand(Role * role, const RoleCommand & command)
    {
        RoleAction roleAction = command.roleAction;
        if (roleAction == RoleAction::ATTACK)
            role->getStateManager()->changeState(AttackState::create());
        else if (roleAction == RoleAction::ATTACKED)
            role->getStateManager()->changeState(AttackedState::create());
    }

}
