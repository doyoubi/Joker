#include "BattleDirector.h"
#include "scene/BattleScene.h"
#include "role/Role.h"
#include "utils/debug.h"

namespace joker
{

    BattleDirector::BattleDirector(BattleScene * battleScene)
        : _battleScene(battleScene),
        _rhythmScript("music/badapple.json"),   // rhythmScript should init first before metronome and eventDispatcher
        _metronome(_rhythmScript.getOffsetRhythmScript(0), 0.04f),
        _nodEventDispatcher(_rhythmScript),
        _hitEventDispatcher(_rhythmScript),
        _missEventDispatcher(_rhythmScript)
    {
        CHECKNULL(battleScene);

        _metronome.setRhythmCallBack([this](int i){
            _nodEventDispatcher.runEvent(i);
        });
        _metronome.setHitCallBack([this](int index, float dt){
            _hitEventDispatcher.runEvent(index);
        });
        _metronome.setMissCallBack([this](int index){
            _missEventDispatcher.runEvent(index);
        });

        _nodEventDispatcher.addEvent(_rhythmScript.getEvent("nod"), [this](){
            sendCommand(getClosestEnemy(), RoleAction::NOD);
        });

        _hitEventDispatcher.addEvent(_rhythmScript.getEvent("attack"), [this](){
            attack(getPlayer(), getClosestEnemy());
        });

        _missEventDispatcher.addEvent(_rhythmScript.getEvent("attack"), [this](){
            attack(getClosestEnemy(), getPlayer());
        });


        _metronome.reset();
        _metronome.start();
    }

    void BattleDirector::sendCommand(Role * role, RoleAction command)
    {
        CHECKNULL(role);
        if (command == RoleAction::NOD)
            role->getStateManager()->changeState(NodState::create());
        else
            role->executeCommand(command);
    }

    Role * BattleDirector::getPlayer()
    {
        return _battleScene->getBattleLayer()->getPlayer();
    }

    void BattleDirector::attack(Role * attacker, Role * sufferer)
    {
        int d = attacker->getPosition().x - sufferer->getPosition().x;
        if (abs(d) < 300)
        {
            attacker->setDirection(d < 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
            attacker->executeCommand(RoleAction::ATTACK);
            sufferer->executeCommand(RoleAction::ATTACKED);
        }
    }

    Role * BattleDirector::getClosestEnemy()
    {
        vector<Role*> & enemyArray = _battleScene->getBattleLayer()->getEnemyArray();
        DEBUGCHECK(enemyArray.size() > 0, "empty enemy array");
        auto it = std::min_element(std::begin(enemyArray), std::end(enemyArray), 
            [this](Role * r, Role * min) {
            return abs(r->getPosition().x - getPlayer()->getPosition().x)
                < abs(min->getPosition().x - getPlayer()->getPosition().x);
        });
        return *it;
    }

    void BattleDirector::tagMetronome()
    {
        _metronome.tab();
    }


}
