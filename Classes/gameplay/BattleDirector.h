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
        void sendCommand(RolePtr & role, const RoleCommand & command);

        void restartMetronome();

        void tagMetronome();

        RhythmEventDispatcher & getEventDispather(const char * eventName);

        void update(float dt);
        DirectorEventManager & getEventManager() { return _eventManager; }
        void addEvent(DirectorEventType event){ _eventManager.activateEvent(event); }

        void addPlayer(const cocos2d::Vec2 & position);
        RolePtr & getPlayer();
        void addEnemy(const cocos2d::Vec2 & position);
        void removeEnemy(RolePtr & enemy);
        RolePtr & getClosestEnemy();
        int getEnemyNum() { return _enemyConductor.getEnemyArray().size(); }

        void supplyEnemy();
        void enemyAttackReady();

        bool withinAttackScope(const RolePtr & attacker, const RolePtr & sufferrer);

        SoundManager * getSoundManager() { return &_soundManager; }

    private:

        SoundManager  _soundManager;

        BattleScene * _battleScene = nullptr;   // weak reference

        // Here we should make sure that _rhyScript init before metronome and eventDispatcher
        // because the latter two member will use _rhythmScript for initialization.
        // The c++ standard said,
        // nonstatic data members shall be initialized in the order
        // they were declared in the class definition
        // regardless of the order of the mem-initializers.
        // So we have to declare RhythmScript member first.
        RhythmScript _rhythmScript;
        Metronome _metronome;
        std::unordered_map<const char*, RhythmEventDispatcher> _rhythmEventDispaters;

        const float _moveToTime;
        RhythmScript _promptScript;
        Metronome _promptMetronome;

        EnemyConductor _enemyConductor;
        DirectorEventManager _eventManager;

        RolePtr _player;

        void operator=(const BattleDirector &) = delete;
        BattleDirector(const BattleScene &) = delete;
    };

}

#endif
