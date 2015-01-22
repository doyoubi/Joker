#include "Event.h"
#include "utils/debug.h"
#include "AI/BehaviorTree.h"
#include "BattleDirector.h"
#include "role/Role.h"
#include "scene/BattleScene.h"

namespace joker
{
    // DirectorEventManager
    DirectorEventManager::DirectorEventManager()
    {
        _eventPool.emplace(DirectorEventType::ATTACK, EventPtr(new AttackEvent()));
        _eventPool.emplace(DirectorEventType::ATTACKED, EventPtr(new AttackedEvent()));
        _eventPool.emplace(DirectorEventType::NOD, EventPtr(new NodEvent()));
        _eventPool.emplace(DirectorEventType::COLLIDE, EventPtr(new CollideEvent()));
    }

    void DirectorEventManager::activateEvent(DirectorEventType event)
    {
        _eventPool.at(event)->activate();
    }

    void DirectorEventManager::executeEvent(BattleDirector * director)
    {
        for (auto & p : _eventPool)
        {
            if (p.second->isActive())
            {
                p.second->execute(director);
                p.second->deactivate();
            }
        }
    }

    bool DirectorEventManager::hasEvent()
    {
        for (auto & p : _eventPool)
        {
            if (p.second->isActive()) return true;
        }
        return false;
    }

    bool DirectorEventManager::isActive(DirectorEventType event)
    {
        return _eventPool.at(event)->isActive();
    }


    // AttackEvent
    void AttackEvent::execute(BattleDirector * director)
    {
        Role * attacker = director->getPlayer();
        Role * sufferer = director->getClosestEnemy();
        int d = attacker->getPosition().x - sufferer->getPosition().x;
        attacker->setDirection(d < 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        attacker->executeCommand(RoleAction::ATTACK);
        sufferer->executeCommand(RoleAction::ATTACKED);

        director->getScene()->getSoundManager()->playSound("hit");
    }

    // AttackedEvent
    void AttackedEvent::execute(BattleDirector * director)
    {
        Role * sufferer = director->getPlayer();
        Role * attacker = director->getClosestEnemy();
        int d = attacker->getPosition().x - sufferer->getPosition().x;
        attacker->setDirection(d < 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        attacker->executeCommand(RoleAction::ATTACK);
        sufferer->executeCommand(RoleAction::ATTACKED);
    }

    // NodEvent
    void NodEvent::execute(BattleDirector * director)
    {
        director->sendCommand(director->getClosestEnemy(), RoleAction::NOD);
    }

    // CollideEvent
    void CollideEvent::execute(BattleDirector * director)
    {
        cout << "collide happen" << endl;
    }

}
