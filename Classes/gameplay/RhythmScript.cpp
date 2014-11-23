#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include "RhythmScript.h"
#include "utils/debug.h"

namespace joker
{
    using namespace std;
    using namespace cocos2d;

    RhythmScript::RhythmScript(const char * scriptFile)
    {
        auto str = String::createWithContentsOfFile(scriptFile);
        using namespace std;
        DEBUGCHECK(str->getCString() != nullptr, "can't open file: " + string(scriptFile));
        stringstream ss(str->getCString());
        string dt;
        while (getline(ss, dt, ','))
        {
            float t = strtof(dt.c_str(), nullptr);
            DEBUGCHECK(t != 0.0f, "parse fail");
            _rhythmScript.push_back(t / 1000.0f);
        }
    }

    vector<float> RhythmScript::getOffsetRhythmScript(float putOff)
    {
        DEBUGCHECK(_rhythmScript.size() > 0, "invalid rhythm script");
        vector<float> ret(_rhythmScript.size());
        ret[0] = std::max(0.0f, _rhythmScript[0] + putOff);
        std::copy(++begin(_rhythmScript), end(_rhythmScript), ++begin(ret));
        return ret;
    }

}
