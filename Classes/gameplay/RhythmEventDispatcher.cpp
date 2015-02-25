#include "RhythmEventDispatcher.h"
#include "utils/debug.h"

namespace joker
{

    RhythmEventDispatcher::RhythmEventDispatcher(const RhythmScript & rhythmScript)
        : _RhythmEventDispatcherCallBackIndices(rhythmScript.getScriptLength())
    {
    }

    void RhythmEventDispatcher::addEvent(vector<int> eventIndices, EventCallBack eventCallBack)
    {
        _eventCallBackArray.push_back(std::move(eventCallBack));
        for (int i : eventIndices)
        {
            DEBUGCHECK(0 <= i && i < _RhythmEventDispatcherCallBackIndices.size(), "invalid eventIndex");
            _RhythmEventDispatcherCallBackIndices[i].push_back(_eventCallBackArray.size() - 1);
        }
    }

    void RhythmEventDispatcher::runEvent(int eventIndex, float dt)
    {
        DEBUGCHECK(0 <= eventIndex && eventIndex < _RhythmEventDispatcherCallBackIndices.size(), "invalid eventIndex");
        for (int i : _RhythmEventDispatcherCallBackIndices[eventIndex])
        {
            _eventCallBackArray[i](dt);
        }
    }

}
