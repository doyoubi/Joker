#ifndef JOKER_BATTLE_DIRECTOR
#define JOKER_BATTLE_DIRECTOR

#include <unordered_map>

#include "role/RoleEnumType.h"
#include "RhythmScript.h"
#include "metronome.h"
#include "RhythmEventDispatcher.h"
#include "gameplay/AI/EnemyConductor.h"


namespace joker
{

    class Role;
    class BattleScene;

    class BattleDirector
    {
    public:
        BattleDirector(BattleScene * battleScene);
        ~BattleDirector();
        void sendCommand(Role * role, RoleAction command);
        Role * getPlayer();

        void restartMetronome();

        void tagMetronome();
        Role * getClosestEnemy();
        void attack(Role * attacker, Role * sufferer);

        RhythmEventDispatcher & getEventDispather(const char * eventName);

        void update(float dt);
        void setBTEvent(BTEvent event) { _behaviorTreeEvent = event; }

    private:
        Role * addEnemy(const cocos2d::Vec2 & position);

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
        std::unordered_map<const char*, RhythmEventDispatcher> _eventDispaters;

        EnemyConductor _enemyConductor;
        BTEvent _behaviorTreeEvent = BTEvent::NO_EVENT;

        void operator=(const BattleDirector &) = delete;
        BattleDirector(const BattleScene &) = delete;
    };

}

#endif
