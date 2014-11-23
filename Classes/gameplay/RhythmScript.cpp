#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include "cocostudio/CocoStudio.h"

#include "RhythmScript.h"
#include "utils/debug.h"

namespace joker
{
    using namespace std;
    using namespace cocos2d;

    RhythmScript::RhythmScript(const char * scriptFile)
    {
        DEBUGCHECK(FileUtils::getInstance()->isFileExist(scriptFile),
            string(scriptFile) + " file not exit or empty");
        string data = FileUtils::getInstance()->getStringFromFile(scriptFile);
        DEBUGCHECK(data.length() != 0, string("empty file: ") + scriptFile);

        using namespace rapidjson;
        Document doc;
        doc.Parse<kParseDefaultFlags>(data.c_str());
        DEBUGCHECK(!doc.HasParseError(),
            string(scriptFile) + ": " + (doc.GetParseError() == nullptr ? "" : doc.GetParseError())
            );
        
        rapidjson::Value & rhythmPoints = doc["rhythmPoints"];
        DEBUGCHECK(!rhythmPoints.IsNull(), "rhythmPoints parsed to null");
        DEBUGCHECK(rhythmPoints.IsArray(), "type of rhythmPoints is not array");
        DEBUGCHECK(rhythmPoints.Size() > 1, "there should be at least one rhythm point");
        for (SizeType i = 0; i < rhythmPoints.Size(); i++)
        {
            DEBUGCHECK(rhythmPoints[i].IsInt(), "data is not int");
            int dt = rhythmPoints[i].GetInt();
            _rhythmScript.push_back(dt / 1000.0f);
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
