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
        _eventPool.emplace(DirectorEventType::COLLIDE_TO_LEFT, EventPtr(new CollideEvent(RoleDirection::LEFT)));
        _eventPool.emplace(DirectorEventType::COLLIDE_TO_RIGHT, EventPtr(new CollideEvent(RoleDirection::RIGHT)));
    }

    void DirectorEventManager::activateEvent(DirectorEventType event)
    {
        DEBUGCHECK(_eventPool.count(event) == 1,
            "event not register in DirectorEventManager conctructor");
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
        RolePtr & attacker = director->getPlayer();
        RolePtr & sufferer = director->getClosestEnemy();
        int d = attacker->getPosition().x - sufferer->getPosition().x;
        attacker->setDirection(d < 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        attacker->executeCommand(RoleCommand(RoleAction::ATTACK));
        sufferer->executeCommand(RoleCommand(RoleAction::ATTACKED));

        director->getSoundManager()->playSound("hit");
        director->removeEnemy(sufferer);

        director->supplyEnemy();
    }

    // AttackedEvent
    void AttackedEvent::execute(BattleDirector * director)
    {
        RolePtr & sufferer = director->getPlayer();
        RolePtr & attacker = director->getClosestEnemy();
        int d = attacker->getPosition().x - sufferer->getPosition().x;
        attacker->setDirection(d < 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        attacker->executeCommand(RoleCommand(RoleAction::ATTACK));
        sufferer->executeCommand(RoleCommand(RoleAction::ATTACKED));
    }

    // NodEvent
    void NodEvent::execute(BattleDirector * director)
    {
        if (director->getEnemyNum() == 0) return;
        director->sendCommand(director->getClosestEnemy(), RoleAction::NOD);
    }

    // CollideEvent
    void CollideEvent::execute(BattleDirector * director)
    {
        RoleCommand command(RoleAction::COLLIDE);
        command.add<RoleDirection>("direction", _direction);
        director->sendCommand(director->getPlayer(), command);
    }

}
