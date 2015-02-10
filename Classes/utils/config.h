#ifndef JOKER_CONFIG
#define JOKER_CONFIG

#include "cocostudio/CocoStudio.h"

#include <initializer_list>

namespace joker
{
    class Config
    {
    public:
        static Config & getInstance();
        float getValue(std::initializer_list<const char*> valueName);
    private:
        Config(const char * configJsonFile);
        rapidjson::Document _jsonDoc;
    };
}

#endif
