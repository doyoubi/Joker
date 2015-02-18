#include "Event.h"
#include "utils/debug.h"
#include "AI/BehaviorTree.h"
#include "BattleDirector.h"
#include "role/Role.h"
#include "scene/BattleScene.h"
#include "utils/config.h"

namespace joker
{
    // DirectorEventManager
    DirectorEventManager::DirectorEventManager()
    {
    }

    void DirectorEventManager::addEvent(EventPtr && event)
    {
        DEBUGCHECK(!_addEventLock, "can't add event while executeEvent() running.");
        _eventPool.push_back(std::move(event));
    }

    void DirectorEventManager::executeEvent(BattleDirector * director)
    {
        _addEventLock = true;
        for (auto & p : _eventPool)
        {
            p->execute(director);
        }
        _eventPool.clear();
        _addEventLock = false;
    }

    bool DirectorEventManager::hasEvent()
    {
        return !_eventPool.empty();
    }


    // PlayerAttackEvent
    void PlayerAttackEvent::execute(BattleDirector * director)
    {
        RolePtr & attacker = director->getPlayer();
        RolePtr & sufferer = director->getClosestEnemy();
        int d = attacker->getPosition().x - sufferer->getPosition().x;
        attacker->setDirection(d < 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        sufferer->setDirection(d < 0 ? RoleDirection::LEFT : RoleDirection::RIGHT);
        sufferer->executeCommand(RoleAction::ATTACKED);

        director->getSoundManager()->playSound("hit");

        director->supplyEnemy();
    }

    // EnemyAttackEvent
    void EnemyAttackEvent::execute(BattleDirector * director)
    {
        RolePtr & sufferer = director->getPlayer();
        RolePtr & attacker = director->getClosestEnemy();
        int d = attacker->getPosition().x - sufferer->getPosition().x;
        attacker->setDirection(d < 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        sufferer->setDirection(d < 0 ? RoleDirection::LEFT : RoleDirection::RIGHT);
        sufferer->executeCommand(RoleAction::ATTACKED);
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

    // EmptyAttackEvent
    void EmptyAttackEvent::execute(BattleDirector * director)
    {
        if (director->getEnemyNum() == 0) return;
        float d = director->getPlayer()->getPosition().x - director->getClosestEnemy()->getPosition().x;
        if (!director->withinAttackScope(director->getPlayer(), director->getClosestEnemy())) return;
        director->sendCommand(director->getClosestEnemy(), RoleAction::DEFENCE);
    }

    // RemoveRoleEvent
    void RemoveRoleEvent::execute(BattleDirector * director)
    {
        if (_role->getRoleType() == RoleType::ENEMY)
            director->removeEnemy(_role);
        else if (_role->getRoleType() == RoleType::BOMB)
            director->removeBomb(_role);
        else ERRORMSG("invalid role type");
    }

}
