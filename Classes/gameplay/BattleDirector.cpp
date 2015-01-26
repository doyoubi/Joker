#include "BattleDirector.h"
#include "scene/BattleScene.h"
#include "role/Role.h"
#include "utils/debug.h"

namespace joker
{

    BattleDirector::BattleDirector(BattleScene * battleScene)
        : _battleScene(battleScene),
        _rhythmScript("music/badapple.json"),   // rhythmScript should init first before metronome and eventDispatcher
        _metronome(_rhythmScript.getOffsetRhythmScript(0), 0.04f)
    {
        CHECKNULL(battleScene);

        _rhythmEventDispaters.emplace("nod", RhythmEventDispatcher(_rhythmScript));
        _rhythmEventDispaters.emplace("hit", RhythmEventDispatcher(_rhythmScript));
        _rhythmEventDispaters.emplace("miss", RhythmEventDispatcher(_rhythmScript));

        _metronome.setRhythmCallBack([this](int i){
            getEventDispather("nod").runEvent(i);
        });
        _metronome.setHitCallBack([this](int index, float dt){
            getEventDispather("hit").runEvent(index);
        });
        _metronome.setMissCallBack([this](int index){
            getEventDispather("miss").runEvent(index);
        });

        getEventDispather("nod").addEvent(_rhythmScript.getEvent("nod"), [this](){
            this->addEvent(DirectorEventType::NOD);
        });

        getEventDispather("hit").addEvent(_rhythmScript.getEvent("attack"), [this](){
            this->addEvent(DirectorEventType::ATTACK);
        });

        getEventDispather("miss").addEvent(_rhythmScript.getEvent("attack"), [this](){
            this->addEvent(DirectorEventType::ATTACKED);
        });

        auto enemy = addEnemy(Vec2(500, 200));
        enemy->getStateManager()->executeCommand(RoleCommand(RoleAction::DEFENCE));
        _battleScene->getBattleLayer()->addPlayer(Vec2(200, 200));
        getPlayer()->setSpeed(200, 20);

        getPlayer()->setCollideCallbak([this](const CollideInfo & collideInfo){
            auto eventType = collideInfo.selfPosition < collideInfo.oppositePosition ?
                DirectorEventType::COLLIDE_TO_LEFT : DirectorEventType::COLLIDE_TO_RIGHT;
            this->addEvent(eventType);
        });

        Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    BattleDirector::~BattleDirector()
    {
        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    }

    void BattleDirector::sendCommand(Role * role, const RoleCommand & command)
    {
        CHECKNULL(role);
        role->executeCommand(command);
    }

    Role * BattleDirector::getPlayer()
    {
        return _battleScene->getBattleLayer()->getPlayer();
    }

    Role * BattleDirector::getClosestEnemy()
    {
        const vector<Role*> & enemyArray = _enemyConductor.getEnemyArray();
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
        enemy->setSpeed(100, 10);
        _enemyConductor.addEnemy(enemy);
        return enemy;
    }

    void BattleDirector::removeEnemy(Role * enemy)
    {
        _enemyConductor.removeEnemy(enemy);
        _battleScene->getBattleLayer()->removeEnemy(enemy);
    }

    RhythmEventDispatcher & BattleDirector::getEventDispather(const char * eventName)
    {
        DEBUGCHECK(_rhythmEventDispaters.count(eventName) == 1, string("event: ") + eventName + " not exist");
        return _rhythmEventDispaters.at(eventName);
    }

    void BattleDirector::update(float dt)
    {
        BTParam param;
        param.closest = true;
        param.playerPosition = getPlayer()->getPosition().x;

        _eventManager.executeEvent(this);
        if (_enemyConductor.getEnemyArray().empty()) return;
        _enemyConductor.tick(getClosestEnemy(), param);
    }

}
