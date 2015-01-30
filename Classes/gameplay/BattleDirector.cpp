#include "BattleDirector.h"
#include "scene/BattleScene.h"
#include "role/Role.h"
#include "utils/debug.h"
#include "SimplePhysics/PhysicsWorld.h"

namespace joker
{

    BattleDirector::BattleDirector(BattleScene * battleScene)
        : _battleScene(battleScene),
        _rhythmScript("music/badapple.json"),   // rhythmScript should init first before metronome and eventDispatcher
        _metronome(_rhythmScript.getOffsetRhythmScript(0), 0.04f),
        _promptScript("music/badapple_prompt.json"),
        _promptMetronome(_promptScript.getOffsetRhythmScript(0), 0.02)
        // here we will not tab promptMetronome, and we don't care hitDeltaTime, we set it to 0.01
    {
        CHECKNULL(battleScene);

        _promptMetronome.setRhythmCallBack([this](int){
            this->getScene()->getPromptBar()->addPromptSprite();
        });

        _rhythmEventDispaters.emplace("nod", RhythmEventDispatcher(_rhythmScript));
        _rhythmEventDispaters.emplace("hit", RhythmEventDispatcher(_rhythmScript));
        _rhythmEventDispaters.emplace("miss", RhythmEventDispatcher(_rhythmScript));
        _rhythmEventDispaters.emplace("addEnemy", RhythmEventDispatcher(_rhythmScript));

        _metronome.setRhythmCallBack([this](int i){
            getEventDispather("nod").runEvent(i);
            getEventDispather("addEnemy").runEvent(i);
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
            getScene()->getPromptBar()->hitSuccess();
        });

        getEventDispather("miss").addEvent(_rhythmScript.getEvent("attack"), [this](){
            this->addEvent(DirectorEventType::ATTACKED);
            getScene()->getPromptBar()->miss();
        });

        getEventDispather("addEnemy").addEvent(_rhythmScript.getEvent("addEnemy"), [this](){
            this->supplyEnemy();
        });

        addEnemy(Vec2(500, 200));
        addPlayer(Vec2(200, 200));

        Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    BattleDirector::~BattleDirector()
    {
        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    }

    void BattleDirector::sendCommand(RolePtr & role, const RoleCommand & command)
    {
        CHECKNULL(role);
        role->executeCommand(command);
    }

    RolePtr & BattleDirector::getClosestEnemy()
    {
        auto & enemyArray = _enemyConductor.getEnemyArray();
        DEBUGCHECK(enemyArray.size() > 0, "empty enemy array");
        auto it = std::min_element(std::begin(enemyArray), std::end(enemyArray), 
            [this](RolePtr & r, RolePtr & min) {
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
        _promptMetronome.reset();
        _promptMetronome.start();
        _battleScene->getSoundManager()->playSound("badapple");
        getScene()->getPromptBar()->clearPromptSprite();
    }

    void BattleDirector::addPlayer(const cocos2d::Vec2 & position)
    {
        auto player = _battleScene->getBattleLayer()->addPlayerSprite(position);
        _player = RolePtr(new Role(player));
        _player->setIsPlayer();
        _player->setSpeed(200, 20);
        _player->setPosition(position);
        _player->getPhysicsBody()->setCollidable(true);
        _player->setCollideCallbak([this](const CollideInfo & collideInfo){
            auto eventType = collideInfo.selfPosition < collideInfo.oppositePosition ?
                DirectorEventType::COLLIDE_TO_LEFT : DirectorEventType::COLLIDE_TO_RIGHT;
            this->addEvent(eventType);
        });
    }

    RolePtr & BattleDirector::getPlayer()
    {
        return _player;
    }

    void BattleDirector::addEnemy(const cocos2d::Vec2 & position)
    {
        auto enemySprite = _battleScene->getBattleLayer()->addEnemySprite(position);
        auto enemy = RolePtr(new Role(enemySprite));
        enemy->setSpeed(100, 10);
        enemy->setPosition(position);
        _enemyConductor.addEnemy(std::move(enemy));
    }

    void BattleDirector::removeEnemy(RolePtr & enemy)
    {
        _enemyConductor.removeEnemy(enemy);
    }

    RhythmEventDispatcher & BattleDirector::getEventDispather(const char * eventName)
    {
        DEBUGCHECK(_rhythmEventDispaters.count(eventName) == 1, string("event: ") + eventName + " not exist");
        return _rhythmEventDispaters.at(eventName);
    }

    void BattleDirector::update(float dt)
    {
        _eventManager.executeEvent(this);
        BTParam param;
        param.playerPosition = getPlayer()->getPosition().x;
        for (RolePtr & enemy : _enemyConductor.getEnemyArray())
        {
            param.closest = enemy == getClosestEnemy();
            _enemyConductor.tick(enemy, param);
        }
    }

    void BattleDirector::supplyEnemy()
    {
        if (_enemyConductor.getEnemyArray().size() > 1) return;
        const int distance = 500;
        float posi = getPlayer()->getPosition().x;
        float delta = distance;
        if (_enemyConductor.getEnemyArray().empty())
        {
            if (posi > distance) delta *= -1;
        }
        else
        {
            RolePtr & closest = getClosestEnemy();
            if (closest->getPosition().x > getPlayer()->getPosition().x)
                delta *= -1;
        }
        const int width = joker::PhysicsWorld::getInstance()->getWorldWidth();
        if (posi + delta < 1.5 * Role::PlayerShortAttackScope
            || posi + delta > width - Role::PlayerShortAttackScope)
            delta *= -1;
        addEnemy(Vec2(posi + delta, 0));
    }

}
