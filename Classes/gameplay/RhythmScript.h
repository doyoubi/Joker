#ifndef JOKER_RHYTHM_SCRIPT
#define JOKER_RHYTHM_SCRIPT

#include <vector>
#include <unordered_map>
#include <string>

#include "cocos2d.h"

namespace joker
{
    class RhythmScript
    {
    public:
        RhythmScript(const char * scriptFile);
        std::vector<float> getOffsetRhythmScript(float putOff);   // putOff can be negative
        std::vector<int> & getEvent(std::string eventName);
        int getScriptLength() const { return _rhythmScript.size(); }

    private:
        std::vector<float> _rhythmScript;
        std::unordered_map<std::string, std::vector<int>> _events;
    };
}

#endif
