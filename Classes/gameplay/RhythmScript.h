#ifndef JOKER_RHYTHM_SCRIPT
#define JOKER_RHYTHM_SCRIPT

#include <vector>

#include "cocos2d.h"

namespace joker
{
    class RhythmScript
    {
    public:
        RhythmScript(const char * scriptFile);
        std::vector<float> getOffsetRhythmScript(float putOff);   // putOff can be negative
        int getScriptLength() const { return _rhythmScript.size(); }

    private:
        std::vector<float> _rhythmScript;
    };
}

#endif
