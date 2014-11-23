#include "BattleDirector.h"
#include "scene/BattleScene.h"
#include "role/Role.h"
#include "utils/debug.h"

namespace joker
{

    BattleDirector::BattleDirector(BattleScene * battleScene)
        : _battleScene(battleScene),
        _rhythmScript("music/badapple.json"),   // rhythmScript should init first before metronome and eventDispatcher
        _metronome(_rhythmScript.getOffsetRhythmScript(0), 0.04f),
        _eventDispatcher(_rhythmScript)
    {
        CHECKNULL(battleScene);
        _metronome.setRhythmCallBack([this](int i){
            _eventDispatcher.runEvent(i);
        });
        _eventDispatcher.addEvent(_rhythmScript.getEvent("nod"), [](){
            cout << "nod" << endl;
        });
        _eventDispatcher.addEvent(_rhythmScript.getEvent("attack"), [](){
            cout << "attack" << endl;
        });
        _metronome.reset();
        _metronome.start();
    }

    void BattleDirector::sendCommand(Role * role, RoleAction command)
    {
        CHECKNULL(role);
        role->executeCommand(command);
    }

    Role * BattleDirector::getPlayer()
    {
        return _battleScene->getBattleLayer()->getPlayer();
    }

}
