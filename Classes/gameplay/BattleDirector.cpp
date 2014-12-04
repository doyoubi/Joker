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
        _missEventDispatcher(_rhythmScript),
        _getCloseDispatcher(_rhythmScript)
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
            _battleScene->getSoundManager()->playSound("hit");
        });

        _missEventDispatcher.addEvent(_rhythmScript.getEvent("attack"), [this](){
            attack(getClosestEnemy(), getPlayer());
        });

        addEnemy(Vec2(500, 200));
        _battleScene->getBattleLayer()->addPlayer(Vec2(200, 200));
        getPlayer()->setSpeed(200, 100);

        _missEventDispatcher.addEvent(_rhythmScript.getEvent("getClose"), [this](){
            this->setBTEvent(BTEvent::ROLE_GET_CLOSE);
            RoleDirection d = getClosestEnemy()->getPosition().x < getPlayer()->getPosition().x ?
                RoleDirection::LEFT : RoleDirection::RIGHT;
            getPlayer()->setDirection(d);
            getPlayer()->executeCommand(RoleAction::READY);
        });

        Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    BattleDirector::~BattleDirector()
    {
        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
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
        attacker->setDirection(d < 0 ? RoleDirection::RIGHT : RoleDirection::LEFT);
        attacker->executeCommand(RoleAction::ATTACK);
        sufferer->executeCommand(RoleAction::ATTACKED);
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

    void BattleDirector::restartMetronome()
    {
        _metronome.reset();
        _metronome.start();
        _battleScene->getSoundManager()->playSound("badapple");
    }

    Role * BattleDirector::addEnemy(const cocos2d::Vec2 & position)
    {
        auto enemy = _battleScene->getBattleLayer()->addEnemy(position);
        _enemyConductor.addEnemy(enemy);
        return enemy;
    }

    void BattleDirector::update(float dt)
    {
        BTParam param;
        param.event = _behaviorTreeEvent;
        param.closest = true;
        param.playerPosition = getPlayer()->getPosition().x;
        _enemyConductor.tick(getClosestEnemy(), param);
        _behaviorTreeEvent = BTEvent::NO_EVENT;
    }

}
