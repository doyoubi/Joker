#ifndef JOKER_RHYTHM_EVENT
#define JOKER_RHYTHM_EVENT

#include <functional>
#include <vector>

#include "RhythmScript.h"

namespace joker
{
    using std::function;
    using std::vector;

    class RhythmEventDispatcher
    {
    public:
        typedef function<void(void)> EventCallBack;

        RhythmEventDispatcher(const RhythmScript & rhythmScript);
        void addEvent(vector<int> eventIndices, EventCallBack eventCallBack);

        void runEvent(int eventIndex);

    private:
        vector<vector<int>> _RhythmEventDispatcherCallBackIndices;  // one rhythm point index  to  multiple event callback
        vector<EventCallBack> _eventCallBackArray;
    };
}

#endif
