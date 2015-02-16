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
        : _battleScene(battleScene)
    {
        CHECKNULL(battleScene);

        _rhythmScripts.emplace("battle", musicScript.scriptName.c_str());
        _metronomes.emplace("battle", 
            Metronome(getScript("battle").getOffsetRhythmScript(0),
            Config::getInstance().getDoubleValue({ "Metronome", "hitDeltaTime" }))
        );
        const float moveToTime = getScript("battle").getOffsetRhythmScript(0)[0] - Config::getInstance().getDoubleValue({ "Metronome", "promptStartTime" });
        _rhythmScripts.emplace("prompt", RhythmScript(musicScript.promptName.c_str()));
        _metronomes.emplace("prompt", Metronome(_rhythmScripts.at("prompt").getOffsetRhythmScript(-1 * moveToTime), 0.02f));
        // here we will not tab promptMetronome, and we don't care hitDeltaTime, we set it to 0.02

        auto physicsWorld = joker::PhysicsWorld::getInstance();
        physicsWorld->setWorldWidth(getScene()->getBattleLayer()->getBackground()->getContentSize().width);
        physicsWorld->setGravity(Config::getInstance().getDoubleValue({"Physics", "gravity"}));
        physicsWorld->setGroundHeight(Config::getInstance().getDoubleValue({ "Physics", "groundHeight" }));
        physicsWorld->setResistance(Config::getInstance().getDoubleValue({ "Physics", "resistance" }));

        getSoundManager()->loadSound("hit", "music/knock.wav");

        getMetronome("prompt").setRhythmCallBack([this, moveToTime](int){
            this->getScene()->getPromptBar()->addPromptSprite(moveToTime);
        });

        _rhythmEventDispaters.emplace("nod", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("hit", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("miss", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("addEnemy", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("emptyHit", RhythmEventDispatcher(getScript("battle")));

        getMetronome("battle").setRhythmCallBack([this](int i){
            getEventDispather("nod").runEvent(i);
            getEventDispather("addEnemy").runEvent(i);
        });
        getMetronome("battle").setHitCallBack([this](int index, float dt){
            getEventDispather("hit").runEvent(index);
        });
        getMetronome("battle").setMissCallBack([this](int index){
            getEventDispather("miss").runEvent(index);
        });
        getMetronome("battle").setStartHitCallBack([this](int index){
            getEventDispather("emptyHit").runEvent(index);
        });
        getMetronome("battle").setWrongHitCallBack([this](int, float){
            this->addEvent(DirectorEventType::EMPTY_HIT);
        });

        getEventDispather("nod").addEvent(getScript("battle").getEvent("nod"), [this](){
            this->addEvent(DirectorEventType::NOD);
            getScene()->getPromptBar()->rhythm();
        });

        getEventDispather("hit").addEvent(getScript("battle").getEvent("attack"), [this](){
            this->addEvent(DirectorEventType::ATTACK);
            getScene()->getPromptBar()->hitSuccess();
        });

        getEventDispather("miss").addEvent(getScript("battle").getEvent("attack"), [this](){
            this->addEvent(DirectorEventType::ATTACKED);
            getScene()->getPromptBar()->miss();
        });

        getEventDispather("addEnemy").addEvent(getScript("battle").getEvent("addEnemy"), [this](){
            this->supplyEnemy();
        });

        getEventDispather("emptyHit").addEvent(getScript("battle").getEvent("attack"), [this](){
            this->enemyAttackReady();
        });

        addEnemy(Vec2(500, 200));
        addPlayer(Vec2(Config::getInstance().getDoubleValue({"RoleProperty", "player", "initPositionX"}),
            Config::getInstance().getDoubleValue({ "RoleProperty", "player", "initPositionY" })));

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
        getMetronome("battle").tab();
    }

    void BattleDirector::restartMetronome()
    {
        getMetronome("battle").reset();
        getMetronome("battle").start();
        getMetronome("prompt").reset();
        getMetronome("prompt").start();
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

    void BattleDirector::enemyAttackReady()
    {
        RolePtr & enemy = getClosestEnemy();
        RoleCommand command(RoleAction::ATTACK_READY);
        enemy->executeCommand(command);
    }

    bool BattleDirector::withinAttackScope(const RolePtr & attacker, const RolePtr & sufferrer)
    {
        static float playerScope = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "attackScope" });
        static float enemyScope = Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "attackScope" });
        float scope = attacker->isPlayer() ? playerScope : enemyScope;
        float attackerX = attacker->getPosition().x;
        float sufferrerX = sufferrer->getPosition().x;
        float d = attackerX - sufferrerX;
        if (std::abs(attackerX - sufferrerX) > scope) return false;
        if (attacker->getDirection() == RoleDirection::LEFT && d > 0)
            return true;
        if (attacker->getDirection() == RoleDirection::RIGHT && d < 0)
            return true;
        return false;
    }

}
