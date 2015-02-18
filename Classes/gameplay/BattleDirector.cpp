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
        _rhythmScripts.emplace("prompt", RhythmScript(musicScript.scriptName.c_str()));
        _metronomes.emplace("prompt", Metronome(getScript("battle").getOffsetRhythmScript(-1 * moveToTime), 0.02f));
        // here we will not tab promptMetronome, and we don't care hitDeltaTime, we set it to 0.02
        _metronomes.emplace("enemyRush", Metronome(getScript("battle").getOffsetRhythmScript(
            -1 * Config::getInstance().getDoubleValue({"EnemyKeepDistance", "rushTime"})), 0.02f));
        const float explodeTime = Config::getInstance().getDoubleValue({ "RoleProperty", "bomb", "explodeTime" });
        _metronomes.emplace("bombFall", Metronome(getScript("battle").getOffsetRhythmScript(-explodeTime), 0.02));

        auto physicsWorld = joker::PhysicsWorld::getInstance();
        physicsWorld->setWorldWidth(getScene()->getBattleLayer()->getBackground()->getContentSize().width);
        physicsWorld->setGravity(Config::getInstance().getDoubleValue({"Physics", "gravity"}));
        physicsWorld->setGroundHeight(Config::getInstance().getDoubleValue({ "Physics", "groundHeight" }));
        physicsWorld->setResistance(Config::getInstance().getDoubleValue({ "Physics", "resistance" }));

        getSoundManager()->loadSound("hit", "music/knock.wav");

        _rhythmEventDispaters.emplace("nod", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("hit", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("miss", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("addEnemy", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("emptyHit", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("enemyRush", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("bombFall", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("bomb", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("attackPrompt", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("bombPrompt", RhythmEventDispatcher(getScript("battle")));

        getMetronome("prompt").setRhythmCallBack([this, moveToTime](int i){
            getEventDispather("attackPrompt").runEvent(i);
            getEventDispather("bombPrompt").runEvent(i);
        });
        getEventDispather("attackPrompt").addEvent(getScript("battle").getEvent("attack"), [this, moveToTime](){
            this->getScene()->getPromptBar()->addPromptSprite(moveToTime, PromptSpriteType::ATTACK);
        });
        getEventDispather("bombPrompt").addEvent(getScript("battle").getEvent("bomb"), [this, moveToTime](){
            this->getScene()->getPromptBar()->addPromptSprite(moveToTime, PromptSpriteType::BOMB);
        });

        getMetronome("battle").setRhythmCallBack([this](int i){
            getEventDispather("nod").runEvent(i);
            getEventDispather("addEnemy").runEvent(i);
            getEventDispather("bomb").runEvent(i);
        });
        getMetronome("battle").setHitCallBack([this](int index, float dt){
            getEventDispather("hit").runEvent(index);
            setBTEvent(BTEvent::NO_EVENT);
        });
        getMetronome("battle").setMissCallBack([this](int index){
            getEventDispather("miss").runEvent(index);
            setBTEvent(BTEvent::NO_EVENT);
        });
        getMetronome("battle").setStartHitCallBack([this](int index){
            getEventDispather("emptyHit").runEvent(index);
        });
        getMetronome("battle").setWrongHitCallBack([this](int, float){
            this->addEvent(EventPtr(new EmptyAttackEvent()));
        });

        // bomb
        getMetronome("bombFall").setRhythmCallBack([this](int i){
            getEventDispather("bombFall").runEvent(i);
        });
        getEventDispather("bombFall").addEvent(getScript("battle").getEvent("bomb"), [this](){
            float x = getPlayer()->getPosition().x;
            this->addBomb(Vec2(x, 600));
        });
        getEventDispather("bomb").addEvent(getScript("battle").getEvent("bomb"), [this](){
            this->getLowestBomb()->executeCommand(RoleAction::EXPLODE);
        });

        // enemy rush
        getMetronome("enemyRush").setRhythmCallBack([this](int index){
            getEventDispather("enemyRush").runEvent(index);
        });
        getEventDispather("enemyRush").addEvent(getScript("battle").getEvent("attack"), [this](){
            this->setBTEvent(BTEvent::READY_TO_ATTACK);
        });

        getEventDispather("nod").addEvent(getScript("battle").getEvent("nod"), [this](){
            this->addEvent(EventPtr(new NodEvent()));
            getScene()->getPromptBar()->rhythm();
        });

        getEventDispather("hit").addEvent(getScript("battle").getEvent("attack"), [this](){
            this->addEvent(EventPtr(new AttackEvent()));
            getScene()->getPromptBar()->hitSuccess();
        });

        getEventDispather("miss").addEvent(getScript("battle").getEvent("attack"), [this](){
            this->addEvent(EventPtr(new AttackedEvent()));
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
        getMetronome("enemyRush").reset();
        getMetronome("enemyRush").start();
        getMetronome("bombFall").reset();
        getMetronome("bombFall").start();
        getSoundManager()->playBackGroundSound(musicScript.musicFileName.c_str());
        getScene()->getPromptBar()->clearPromptSprite();
    }

    void BattleDirector::addPlayer(const cocos2d::Vec2 & position)
    {
        auto player = _battleScene->getBattleLayer()->addPlayerSprite(position);
        int width = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "width" });
        int height = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "height" });
        float scale = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "spriteScale" });
        _player = RolePtr(new Role(this, player,width, height, scale, RoleType::PLAYER));
        _player->setSpeed(Config::getInstance().getDoubleValue({"RoleProperty", "player", "normalSpeed"}),
            Config::getInstance().getDoubleValue({ "RoleProperty", "player", "slowSpeed" }));
        _player->setPosition(position);
        _player->getPhysicsBody()->setCollidable(true);
        _player->setCollideCallbak([this](const CollideInfo & collideInfo){
            auto direction = collideInfo.selfPosition < collideInfo.oppositePosition ?
                RoleDirection::LEFT: RoleDirection::RIGHT;
            this->addEvent(EventPtr(new CollideEvent(direction)));
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
        auto enemy = RolePtr(new Role(this, enemySprite, width, height, scale, RoleType::ENEMY));
        enemy->setSpeed(Config::getInstance().getDoubleValue({"RoleProperty", "enemy", "normalSpeed"}),
            Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "slowSpeed" }));
        enemy->setPosition(position);
        _enemyConductor.addEnemy(std::move(enemy));
    }

    void BattleDirector::removeEnemy(Role * enemy)
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
        param.event = _btEvent;
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

    RhythmScript & BattleDirector::getScript(const char * key)
    {
        DEBUGCHECK(_rhythmScripts.count(key) == 1, string("getScript(): invalid key: ") + key);
        return _rhythmScripts.at(key); 
    }

    Metronome & BattleDirector::getMetronome(const char * key) 
    { 
        DEBUGCHECK(_metronomes.count(key) == 1, string("getMetronome(): invalid key: ") + key);
        return _metronomes.at(key);
    }

    void BattleDirector::addBomb(const cocos2d::Vec2 & position)
    {
        auto bombSprite = _battleScene->getBattleLayer()->addBombSprite(position);
        int width = Config::getInstance().getDoubleValue({ "RoleProperty", "bomb", "width" });
        int height = Config::getInstance().getDoubleValue({ "RoleProperty", "bomb", "height" });
        float scale = Config::getInstance().getDoubleValue({ "RoleProperty", "bomb", "spriteScale" });
        auto bomb = RolePtr(new Role(this, bombSprite, width, height, scale, RoleType::BOMB));
        bomb->setSpeed(Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "normalSpeed" }),
            Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "slowSpeed" }));
        bomb->setPosition(position);
        bomb->getPhysicsBody()->setVelocityY(Config::getInstance().getDoubleValue({"RoleProperty", "bomb", "vy"}));
        int direction = getPlayer()->getDirection() == RoleDirection::LEFT ? -1 : 1;
        bomb->getPhysicsBody()->setVelocityX(Config::getInstance().getDoubleValue({ "RoleProperty", "bomb", "vxAbs" }) * direction);
        bomb->executeCommand(RoleAction::FALLING);
        _bombs.push_back(std::move(bomb));
    }

    void BattleDirector::removeBomb(Role * bomb)
    {
        CHECKNULL(bomb);
        auto it = std::find_if(begin(_bombs), end(_bombs), [bomb](RolePtr & ptr){ return ptr.get() == bomb; });
        DEBUGCHECK(end(_bombs) != it, "enemy not exist in EnemyConductor");
        _bombs.erase(it);
    }

    RolePtr & BattleDirector::getLowestBomb()
    {
        DEBUGCHECK(!_bombs.empty(), "no bomb exist");
        auto it = std::min_element(begin(_bombs), end(_bombs), [](RolePtr & lhs, RolePtr & rhs){
            return lhs->getPhysicsBody()->getHeight() < rhs->getPhysicsBody()->getHeight();
        });
        return *it;
    }


}
