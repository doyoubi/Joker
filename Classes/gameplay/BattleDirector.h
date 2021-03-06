#ifndef JOKER_BATTLE_DIRECTOR
#define JOKER_BATTLE_DIRECTOR

#include <unordered_map>
#include <memory>
#include <vector>

#include "role/RoleEnumType.h"
#include "RhythmScript.h"
#include "metronome.h"
#include "RhythmEventDispatcher.h"
#include "gameplay/AI/EnemyConductor.h"
#include "Event.h"
#include "role/Role.h"
#include "sound/SoundManager.h"
#include "AI/BehaviorTree.h"
#include "BattleJudge.h"
#include "SimplePhysics/PhysicsWorld.h"


namespace joker
{

    struct RoleCommand;
    class BattleScene;

    class BattleDirector
    {
    public:

        BattleDirector(BattleScene * battleScene);
        ~BattleDirector();
        BattleScene * getScene() { return _battleScene; }
        void sendCommand(Role * role, const RoleCommand & command);

        Metronome & getMetronome(const char * key);
        void startBattle();
        void tabMetronome();

        RhythmEventDispatcher & getEventDispather(const char * eventName);

        void update(float dt);
        DirectorEventManager & getEventManager() { return _eventManager; }
        void addEvent(EventPtr && event){ _eventManager.addEvent(std::move(event)); }

        BattleJudge & getBattleJudge() { return _battleJudge; }

        void addPlayer(const cocos2d::Vec2 & position);
        Role * getPlayer();
        void addEnemy(const cocos2d::Vec2 & position);
        void removeEnemy(Role * enemy);
        Role * getClosestEnemy();
        vector<Role*> getEnemyArray(){ return _enemyConductor.getEnemyArray(); }
        int getEnemyNum() { return _enemyConductor.getEnemyArray().size(); }
        void addBomb(const cocos2d::Vec2 & position);
        void removeBomb(Role * bomb);

        void supplyEnemy();
        void enemyAttackReady();

        bool withinAttackScope(const Role * attacker, const Role * sufferrer);

        SoundManager * getSoundManager() { return &_soundManager; }

        void setBTEvent(BTEvent event) { _btEvent = event; }

        RolePtr & getLowestBomb();
        void endBattle();
        bool isBattleEnded() const { return _battleEnded; }

    private:

        SoundManager  _soundManager;
        joker::PhysicsWorld _physicsWorld;

        BattleScene * _battleScene = nullptr;   // weak reference

        std::unordered_map<const char*, RhythmEventDispatcher> _rhythmEventDispaters;

        std::unordered_map<std::string, RhythmScript> _rhythmScripts;
        std::unordered_map<std::string, Metronome> _metronomes;
        RhythmScript & getScript(const char * key);

        EnemyConductor _enemyConductor;
        DirectorEventManager _eventManager;
        BattleJudge _battleJudge;

        RolePtr _player;

        BTEvent _btEvent = BTEvent::NO_EVENT;
        bool _musicStarted = false;
        bool _battleEnded = false;

        vector<RolePtr> _bombs;

        void operator=(const BattleDirector &) = delete;
        BattleDirector(const BattleScene &) = delete;
    };

}

#endif
