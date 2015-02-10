#include <limits>

#include "cocos2d.h"

#include "config.h"
#include "debug.h"

namespace joker
{
    Config & Config::getInstance()
    {
        static Config config("config.json");
        return config;
    }

    Config::Config(const char * configJsonFile)
    {
        DEBUGCHECK(FileUtils::getInstance()->isFileExist(configJsonFile),
            string(configJsonFile) + " file not exit or empty");
        string data = FileUtils::getInstance()->getStringFromFile(configJsonFile);
        DEBUGCHECK(data.length() != 0, string("empty file: ") + configJsonFile);

        using namespace rapidjson;
        _jsonDoc.Parse<kParseDefaultFlags>(data.c_str());
        DEBUGCHECK(!_jsonDoc.HasParseError(),
            string(configJsonFile) + ": " + (_jsonDoc.GetParseError() == nullptr ? "" : _jsonDoc.GetParseError())
            );
    }

    float Config::getValue(std::initializer_list<const char*> valueName)
    {
        rapidjson::Value * v = &_jsonDoc;
        string fullPathForDebug;
        for (const char * s : valueName)
            fullPathForDebug += string(":") + string(s);
        for (const char * s : valueName)
        {
            v = &((*v)[s]);
            DEBUGCHECK(!v->IsNull(),
                string("config json: no such key: ") + s + ". Required full path is: " + fullPathForDebug);
            if (v->IsDouble())
            {
                return float(v->GetDouble());
            }
            else if (v->IsInt())
            {
                return float(v->GetInt());
            }
            else if(v->IsObject())
            {
                continue;
            }
            else ERRORMSG("config json: use unsupported json type, or path in programm is not complete");
        }
        return std::numeric_limits<float>::quiet_NaN();
    }
}
