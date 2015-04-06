#ifndef JOKER_ANIMATION_SPRITE
#define JOKER_ANIMATION_SPRITE

#include <functional>
#include <string>

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"

namespace joker
{
    class AnimationSprite : public cocos2d::Node
    {
    public:
        static AnimationSprite * create(const std::string & animationName, const std::string & exportJsonFile);
        void playAnimAction(const std::string & animActionName);
        void pause();
        void resume();
        // if allAction is true, then actionName is not used and the complete of all action will trigger callback
        void setActionCompleteCallback(std::string actionName, std::function<void(void)> callback, bool allAction = false);
    private:
        cocostudio::Armature * _armature;
    };
}

#endif
