#ifndef JOKER_SOUND_MANAGER
#define JOKER_SOUND_MANAGER

#include <unordered_map>

#include "SimpleAudioEngine.h"

namespace joker
{

    class SoundManager
    {
    public:
        void loadSound(const char * soundName, const char * soundFile);
        void playSound(const char * soundName);
        void playBackGroundSound(const char * soundName);
    private:
        std::unordered_map<const char *, const char *> _name2file;
        std::unordered_map<const char *, unsigned int> _name2id;
    };

}

#endif
