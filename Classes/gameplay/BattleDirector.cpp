#include <string>

#include "BattleDirector.h"
#include "scene/BattleScene.h"
#include "role/Role.h"
#include "utils/debug.h"
#include "SimplePhysics/PhysicsWorld.h"
#include "utils/config.h"

namespace joker
{
    using std::string;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    const string musicFmt = ".ogg";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    const string musicFmt = ".wav";
#else
    ERRORMSG("music for current platform is not supported");
#endif

    struct MusicScriptFile
    {
        MusicScriptFile(const string path, const string & name)
        : musicName(name), musicFileName(path + name + musicFmt),
        scriptName(path + name + ".json"), promptName(path + name + "_prompt.json")
        {
        }
        const string musicName;
        const string musicFileName;
        const string scriptName;
        const string promptName;
        float moveToTime;
    };
    MusicScriptFile musicScript("music/", "alice");

    BattleDirector::BattleDirector(BattleScene * battleScene)
        : _battleScene(battleScene),
        _rhythmScript(musicScript.scriptName.c_str()),   // rhythmScript should init first before metronome and eventDispatcher
        _metronome(_rhythmScript.getOffsetRhythmScript(0), Config::getInstance().getDoubleValue({"Metronome", "hitDeltaTime"})),
        _promptScript(musicScript.promptName.c_str()),
        _promptMetronome(_promptScript.getOffsetRhythmScript(0), 0.02f)
        // here we will not tab promptMetronome, and we don't care hitDeltaTime, we set it to 0.02
    {
        CHECKNULL(battleScene);

        getSoundManager()->loadSound("hit", "music/knock.wav");

        _promptMetronome.setRhythmCallBack([this](int){
            this->getScene()->getPromptBar()->addPromptSprite(
                Config::getInstance().getDoubleValue({ "Metronome", "promptSpriteMoveTime" }));
            // this (in second) must be equivalent to move_to_time (in millisecond) defined in Resources/music/gen_prompt.py
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
            getScene()->getPromptBar()->rhythm();
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
        getSoundManager()->playBackGroundSound(musicScript.musicFileName.c_str());
        getScene()->getPromptBar()->clearPromptSprite();
    }

    void BattleDirector::addPlayer(const cocos2d::Vec2 & position)
    {
        auto player = _battleScene->getBattleLayer()->addPlayerSprite(position);
        int width = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "width" });
        int height = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "height" });
        float scale = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "spriteScale" });
        _player = RolePtr(new Role(player,width, height, scale));
        _player->setIsPlayer();
        _player->setSpeed(Config::getInstance().getDoubleValue({"RoleProperty", "player", "normalSpeed"}),
            Config::getInstance().getDoubleValue({ "RoleProperty", "player", "slowSpeed" }));
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
        int width = Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "width" });
        int height = Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "height" });
        float scale = Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "spriteScale" });
        auto enemy = RolePtr(new Role(enemySprite, width, height, scale));
        enemy->setSpeed(Config::getInstance().getDoubleValue({"RoleProperty", "enemy", "normalSpeed"}),
            Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "slowSpeed" }));
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
        param.playerWidth = getPlayer()->getPhysicsBody()->getWidth();
        param.playerPosition = getPlayer()->getPosition().x;
        for (RolePtr & enemy : _enemyConductor.getEnemyArray())
        {
            param.closest = enemy == getClosestEnemy();
            _enemyConductor.tick(enemy, param);
        }
    }

    void BattleDirector::supplyEnemy()
    {
        static int maxEnemyNum = Config::getInstance().getDoubleValue({"RoleProperty", "enemy", "maxQuantity"});
        if (_enemyConductor.getEnemyArray().size() >= maxEnemyNum) return;
        const int width = joker::PhysicsWorld::getInstance()->getWorldWidth();
        int posi = getPlayer()->getPosition().x;
        int randomDirection = posi % 2 == 0 ? 1 : -1;
        int distance = randomDirection * Config::getInstance().getDoubleValue({"EnemyApearPosition", "distance"});
        if (posi + distance <= 0 || posi + distance > width)
            distance *= -1;
        addEnemy(Vec2(posi + distance, 0));
    }

}
