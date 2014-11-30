#include <string>

#include "SoundManager.h"

#include "utils/debug.h"


namespace joker
{
    using std::string;
    using namespace CocosDenshion;

    void SoundManager::loadSound(const char * soundName, const char * soundFile)
    {
        SimpleAudioEngine::getInstance()->preloadEffect(soundFile);
        _map.emplace(soundName, soundFile);
    }

    void SoundManager::playSound(const char * soundName)
    {
        DEBUGCHECK(_map.count(soundName) != 0, string(soundName) + " not exist");
        const char * soundFile = _map.at(soundName);
        CHECKNULL(soundFile);
        SimpleAudioEngine::getInstance()->playEffect(soundFile);
    }

}
