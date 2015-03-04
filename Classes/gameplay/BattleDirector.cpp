#include <string>
#include <limits>

#include "BattleDirector.h"
#include "scene/BattleScene.h"
#include "role/Role.h"
#include "utils/debug.h"
#include "SimplePhysics/PhysicsWorld.h"
#include "utils/config.h"

namespace joker
{
    using std::string;


    struct MusicScript
    {
        static const string getMusicFileName()
        {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
            static string file = Config::getInstance().getStringValue({"MusicScript", "androidMusicFile"});
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
            static string file = Config::getInstance().getStringValue({"MusicScript", "winMusicFile"});
#else
            ERRORMSG("music for current platform is not supported");
#endif
            return file;
        }
        static const string getScriptName()
        {
            static string file = Config::getInstance().getStringValue({ "MusicScript", "script" });
            return file;
        }
    };


    BattleDirector::BattleDirector(BattleScene * battleScene)
        : _battleScene(battleScene)
    {
        CHECKNULL(battleScene);

        getBattleJudge().hpChangedCallback = [this](int hp){
            getScene()->getHpBar()->changeHeartNum(hp);
            if (hp == 0) endBattle();
        };
        getBattleJudge().scoreChangedCallback = [this](int score){
            getScene()->getScoreDisplayer()->changeScore(score);
        };

        auto physicsWorld = joker::PhysicsWorld::getInstance();
        physicsWorld->setWorldWidth(getScene()->getBattleLayer()->getSize().width);
        physicsWorld->setGravity(Config::getInstance().getDoubleValue({ "Physics", "gravity" }));
        physicsWorld->setGroundHeight(Config::getInstance().getDoubleValue({ "Physics", "groundHeight" }));
        physicsWorld->setResistance(Config::getInstance().getDoubleValue({ "Physics", "resistance" }));

        const float hitDeltaTime = Config::getInstance().getDoubleValue({ "Metronome", "hitDeltaTime" });
        _rhythmScripts.emplace("battle", MusicScript::getScriptName().c_str());
        _metronomes.emplace("battle", Metronome(getScript("battle").getOffsetRhythmScript(0), hitDeltaTime));
        const float moveToTime = getScript("battle").getOffsetRhythmScript(0)[0] - Config::getInstance().getDoubleValue({ "Metronome", "promptStartTime" });
        _rhythmScripts.emplace("prompt", RhythmScript(MusicScript::getScriptName().c_str()));
        _metronomes.emplace("prompt", Metronome(getScript("battle").getOffsetRhythmScript(-1 * moveToTime), 0.02f));
        // here we will not tab promptMetronome, and we don't care hitDeltaTime, we set it to 0.02
        _metronomes.emplace("enemyRush", Metronome(getScript("battle").getOffsetRhythmScript(
            -1 * Config::getInstance().getDoubleValue({"EnemyKeepDistance", "rushTime"})), 0.02f));
        const float explodeTime = Config::getInstance().getDoubleValue({ "RoleProperty", "bomb", "explodeTime" });
        DEBUGCHECK(explodeTime > 0, "explodeTime is the time from bomb arise to explode, explodeTime must be positive");
        _metronomes.emplace("bombFall", Metronome(getScript("battle").getOffsetRhythmScript(-explodeTime), 0.02));
        const float spikeAriseTime = Config::getInstance().getDoubleValue({ "RoleProperty", "spike", "spikeAriseTime" });
        DEBUGCHECK(spikeAriseTime < 0, "spikeAriseTime must be negative(spike arise before attack)");
        _metronomes.emplace("spikeArise", Metronome(getScript("battle").getOffsetRhythmScript(spikeAriseTime), 0.02));
        _metronomes.emplace("spikeAttack", Metronome(getScript("battle").getOffsetRhythmScript(0.0f), hitDeltaTime));

        getSoundManager()->loadSound("hit", "music/knock.wav");

        _rhythmEventDispaters.emplace("nod", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("hit", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("miss", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("addEnemy", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("enemyRush", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("bombFall", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("bomb", RhythmEventDispatcher(getScript("battle")));

        _rhythmEventDispaters.emplace("attackPrompt", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("bombPrompt", RhythmEventDispatcher(getScript("battle")));

        _rhythmEventDispaters.emplace("spikeArise", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("spikeAttack", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("spikePrompt", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("spikeHit", RhythmEventDispatcher(getScript("battle")));
        _rhythmEventDispaters.emplace("spikeMiss", RhythmEventDispatcher(getScript("battle")));

        float zeroDt = 0.0f;
        float nan = std::numeric_limits<float>::quiet_NaN();
        // prompt
        getMetronome("prompt").setRhythmCallBack([this, zeroDt](int i){
            getEventDispather("attackPrompt").runEvent(i, zeroDt);
            getEventDispather("bombPrompt").runEvent(i, zeroDt);
            getEventDispather("spikePrompt").runEvent(i, zeroDt);
        });
        getEventDispather("attackPrompt").addEvent(getScript("battle").getEvent("attack"), [this, moveToTime](float){
            this->getScene()->getPromptBar()->addPromptSprite(moveToTime, PromptSpriteType::ATTACK);
        });
        getEventDispather("bombPrompt").addEvent(getScript("battle").getEvent("bomb"), [this, moveToTime](float){
            this->getScene()->getPromptBar()->addPromptSprite(moveToTime, PromptSpriteType::BOMB);
        });
        getEventDispather("spikePrompt").addEvent(getScript("battle").getEvent("spikeAttack"), [this, moveToTime](float){
            this->getScene()->getPromptBar()->addPromptSprite(moveToTime, PromptSpriteType::SPIKE);
        });

        getMetronome("battle").setRhythmCallBack([this, zeroDt](int i){
            getEventDispather("nod").runEvent(i, zeroDt);
            getEventDispather("addEnemy").runEvent(i, zeroDt);
            getEventDispather("bomb").runEvent(i, zeroDt);
        });
        getMetronome("battle").setHitCallBack([this](int index, float dt){
            getEventDispather("hit").runEvent(index, dt);
            setBTEvent(BTEvent::NO_EVENT);
        });
        getMetronome("battle").setMissCallBack([this, nan](int index){
            getEventDispather("miss").runEvent(index, nan);
            setBTEvent(BTEvent::NO_EVENT);
        });
        getMetronome("battle").setWrongHitCallBack([this](int, float){
            sendCommand(getPlayer(), RoleAction::EMPTY_ATTACK);
        });

        // spike
        getMetronome("spikeArise").setRhythmCallBack([this, zeroDt](int i){
            getEventDispather("spikeArise").runEvent(i, zeroDt);
        });
        getEventDispather("spikeArise").addEvent(getScript("battle").getEvent("spikeAttack"), [this](float){
            getScene()->getBattleLayer()->spikeArise(getPlayer()->getPosition());
        });

        getMetronome("spikeAttack").setRhythmCallBack([this, zeroDt](int i){
            getEventDispather("spikeAttack").runEvent(i, zeroDt);
        });
        getEventDispather("spikeAttack").addEvent(getScript("battle").getEvent("spikeAttack"), [this](float){
            getScene()->getBattleLayer()->spikeAttack();
        });
        // spike hit
        getMetronome("spikeAttack").setHitCallBack([this](int i, float dt){
            getEventDispather("spikeHit").runEvent(i, dt);
        });
        getEventDispather("spikeHit").addEvent(getScript("battle").getEvent("spikeAttack"), [this](float dt){
            getScene()->getPromptBar()->hit(BattleJudge::time2HitResult(dt));
            getBattleJudge().applyResult(BattleJudge::time2HitResult(dt));
        });
        // spike miss
        getMetronome("spikeAttack").setMissCallBack([this, nan](int i){
            getEventDispather("spikeMiss").runEvent(i, nan);
        });
        getEventDispather("spikeMiss").addEvent(getScript("battle").getEvent("spikeAttack"), [this](float){
            getEventManager().addEvent(EventPtr(new AttackedBySpikeEvent()));
            getScene()->getPromptBar()->hit(HitResult::MISS);
            getBattleJudge().applyResult(HitResult::MISS);
        });

        // bomb
        getMetronome("bombFall").setRhythmCallBack([this, zeroDt](int i){
            getEventDispather("bombFall").runEvent(i, zeroDt);
        });
        getEventDispather("bombFall").addEvent(getScript("battle").getEvent("bomb"), [this](float){
            float x = getPlayer()->getPosition().x;
            this->addBomb(Vec2(x, 600));
        });
        getEventDispather("bomb").addEvent(getScript("battle").getEvent("bomb"), [this](float){
            this->getLowestBomb()->executeCommand(RoleAction::EXPLODE);
            getScene()->getPromptBar()->hit(HitResult::BOMB);
        });

        // enemy rush
        getMetronome("enemyRush").setRhythmCallBack([this, zeroDt](int index){
            getEventDispather("enemyRush").runEvent(index, zeroDt);
        });
        getEventDispather("enemyRush").addEvent(getScript("battle").getEvent("attack"), [this](float){
            this->setBTEvent(BTEvent::READY_TO_ATTACK);
        });

        // battle
        getEventDispather("hit").addEvent(getScript("battle").getEvent("attack"), [this](float dt){
            getPlayer()->executeCommand(RoleAction::ATTACK);
            getScene()->getPromptBar()->hit(BattleJudge::time2HitResult(dt));
            getBattleJudge().applyResult(BattleJudge::time2HitResult(dt));
        });

        getEventDispather("miss").addEvent(getScript("battle").getEvent("attack"), [this](float){
            getClosestEnemy()->executeCommand(RoleAction::ATTACK);
            getScene()->getPromptBar()->hit(HitResult::MISS);
            getBattleJudge().applyResult(HitResult::MISS);
        });

        getEventDispather("addEnemy").addEvent(getScript("battle").getEvent("addEnemy"), [this](float){
            this->supplyEnemy();
        });

        addEnemy(Vec2(500, 200));
        addPlayer(Vec2(Config::getInstance().getDoubleValue({"RoleProperty", "player", "initPositionX"}),
            Config::getInstance().getDoubleValue({ "RoleProperty", "player", "initPositionY" })));

        Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    BattleDirector::~BattleDirector()
    {
        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
        getSoundManager()->stopBackgroundSound();
    }

    void BattleDirector::sendCommand(Role * role, const RoleCommand & command)
    {
        if (_battleEnded) return;
        CHECKNULL(role);
        role->executeCommand(command);
    }

    Role * BattleDirector::getClosestEnemy()
    {
        auto enemyArray = _enemyConductor.getEnemyArray();
        DEBUGCHECK(enemyArray.size() > 0, "empty enemy array");
        auto it = std::min_element(std::begin(enemyArray), std::end(enemyArray), 
            [this](Role * r, Role * min) {
            return abs(r->getPosition().x - getPlayer()->getPosition().x)
                < abs(min->getPosition().x - getPlayer()->getPosition().x);
        });
        return *it;
    }

    void BattleDirector::tabMetronome()
    {
        getMetronome("battle").tab();
    }

    void BattleDirector::restartMetronome()
    {
        for (auto & kv : _metronomes)
        {
            kv.second.reset();
            kv.second.start();
        }
        getSoundManager()->playBackGroundSound(MusicScript::getMusicFileName().c_str());
        getScene()->getPromptBar()->clearPromptSprite();
        _musicStarted = true;
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
        _player->getPhysicsBody()->setJumpCallback([this](){
            this->getMetronome("spikeAttack").tab();
        });
    }

    Role * BattleDirector::getPlayer()
    {
        return _player.get();
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

        if (_battleEnded) return;
        if (_musicStarted && !getSoundManager()->isBackgroundPlaying())
        {
            _musicStarted = false;
            getScene()->showResult(getBattleJudge().getScore());
            _battleEnded = true;
            return;
        }

        BTParam param;
        param.playerWidth = getPlayer()->getPhysicsBody()->getWidth();
        param.playerPosition = getPlayer()->getPosition().x;
        param.event = _btEvent;
        for (Role * enemy : _enemyConductor.getEnemyArray())
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
        Role * enemy = getClosestEnemy();
        RoleCommand command(RoleAction::ATTACK_READY);
        enemy->executeCommand(command);
    }

    bool BattleDirector::withinAttackScope(const Role * attacker, const Role * sufferrer)
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
            return !rhs->isAlive()
             || (lhs->isAlive() && lhs->getPhysicsBody()->getHeight() < rhs->getPhysicsBody()->getHeight());
        });
        return *it;
    }

    void BattleDirector::endBattle()
    {
        _battleEnded = true;
        auto enemys = _enemyConductor.getEnemyArray();
        for (auto & enemy : enemys)
            enemy->executeCommand(RoleAction::IDLE);
        for (auto & metro : _metronomes)
            metro.second.stop();
        getScene()->endBattle();
        _player.reset(nullptr);
    }


}
