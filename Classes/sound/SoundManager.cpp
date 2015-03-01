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
        _name2file.emplace(soundName, soundFile);
    }

    void SoundManager::playSound(const char * soundName)
    {
        DEBUGCHECK(_name2file.count(soundName) != 0, string(soundName) + " not exist");
        const char * soundFile = _name2file.at(soundName);
        CHECKNULL(soundFile);
        if (_name2id.count(soundName) != 0)
        {
            SimpleAudioEngine::getInstance()->stopEffect(_name2id.at(soundName));
            _name2id.erase(soundName);
        }
        unsigned int id = SimpleAudioEngine::getInstance()->playEffect(soundFile);
        _name2id.emplace(soundName, id);
    }

    void SoundManager::playBackGroundSound(const char * soundName)
    {
        DEBUGCHECK(FileUtils::getInstance()->isFileExist(soundName), string(soundName) + " file not exist");
        SimpleAudioEngine::getInstance()->playBackgroundMusic(soundName);
    }

}
