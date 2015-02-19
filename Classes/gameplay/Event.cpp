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
        Role * attacker = director->getPlayer();
        Role* sufferer = director->getClosestEnemy();
        int d = attacker->getPosition().x - sufferer->getPosition().x;
        attacker->setDirection(d < 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        sufferer->setDirection(d < 0 ? RoleDirection::LEFT : RoleDirection::RIGHT);
        RoleCommand command(RoleAction::ATTACKED);
        command.add("direction", sufferer->getDirection());
        sufferer->executeCommand(command);

        director->getSoundManager()->playSound("hit");

        director->supplyEnemy();
    }

    // EnemyAttackEvent
    void EnemyAttackEvent::execute(BattleDirector * director)
    {
        Role * sufferer = director->getPlayer();
        Role * attacker = director->getClosestEnemy();
        int d = attacker->getPosition().x - sufferer->getPosition().x;
        attacker->setDirection(d < 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        sufferer->setDirection(d < 0 ? RoleDirection::LEFT : RoleDirection::RIGHT);
        RoleCommand command(RoleAction::ATTACKED);
        command.add("direction", sufferer->getDirection());
        sufferer->executeCommand(command);
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
        vector<Role*> enemyArray = director->getEnemyArray();
        float playerX = director->getPlayer()->getPosition().x;
        std::sort(begin(enemyArray), end(enemyArray), [playerX](Role * lhs, Role * rhs){
            return abs(lhs->getPosition().x - playerX) < abs(rhs->getPosition().x - playerX);
        });
        for (auto enemy : enemyArray)
        {
            if (!director->withinAttackScope(director->getPlayer(), enemy)) continue;
            director->sendCommand(enemy, RoleAction::DEFENCE);
        }
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

    // SpikeAttackEvent
    void SpikeAttackEvent::execute(BattleDirector * director)
    {
        cout << "spike attack!" << endl;
    }

    // AttackedBySpikeEvent
    void AttackedBySpikeEvent::execute(BattleDirector * director)
    {
        RoleCommand command(RoleAction::ATTACKED);
        command.add("direction", director->getPlayer()->getDirection());
        director->sendCommand(director->getPlayer(), command);
    }

}
