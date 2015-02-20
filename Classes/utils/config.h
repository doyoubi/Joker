#ifndef JOKER_CONFIG
#define JOKER_CONFIG

#include "cocostudio/CocoStudio.h"

#include <initializer_list>
#include <string>

namespace joker
{
    // should not use Config to initialize static or global variable
    // since at that time cocos is not initialized
    class Config
    {
    public:
        static Config & getInstance();
        float getDoubleValue(std::initializer_list<const char*> valueName);
        std::string getStringValue(std::initializer_list<const char*> valueName);
        static void setUsable() { _usableTag = true; }
    private:
        Config(const char * configJsonFile);
        rapidjson::Document _jsonDoc;
        static bool _usableTag;
    };
}

#endif
