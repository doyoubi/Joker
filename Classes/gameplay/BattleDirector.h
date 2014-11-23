#ifndef JOKER_BATTLE_DIRECTOR
#define JOKER_BATTLE_DIRECTOR

#include "role/RoleEnumType.h"
#include "RhythmScript.h"
#include "metronome.h"
#include "RhythmEventDispatcher.h"

namespace joker
{

    class Role;
    class BattleScene;

    class BattleDirector
    {
    public:
        BattleDirector(BattleScene * battleScene);
        void sendCommand(Role * role, RoleAction command);
        Role * getPlayer();
    private:
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
        RhythmEventDispatcher _eventDispatcher;

        void operator=(const BattleDirector &) = delete;
        BattleDirector(const BattleScene &) = delete;
    };

}

#endif
