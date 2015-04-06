#ifndef JOKER_ANIMATION_SPRITE
#define JOKER_ANIMATION_SPRITE

#include <functional>
#include <string>

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"

namespace joker
{
    class AnimationSprite;

    class AnimationSpriteProxy
    {
    public:
        virtual void playAnimAction(const std::string & animActionName);
        virtual void pause();
        virtual void resume();
        virtual void setActionCompleteCallback(
            std::string actionName, std::function<void(void)> callback, bool allAction = false);
        virtual bool isPlaying();
        virtual bool isComplete();
    protected:
        virtual AnimationSprite * getAnimationSprite() = 0;
    };

    class AnimationSprite : public cocos2d::Node, public AnimationSpriteProxy
    {
    public:
        static AnimationSprite * create(const std::string & animationName, const std::string & exportJsonFile);
        void playAnimAction(const std::string & animActionName) override;
        void pause() override;
        void resume() override;
        bool isPlaying() override;
        bool isComplete() override;
        // if allAction is true, then actionName is not used and the complete of all action will trigger callback
        void setActionCompleteCallback(std::string actionName, std::function<void(void)> callback, bool allAction = false) override;
        virtual AnimationSprite * getAnimationSprite() { return this; }
    private:
        cocostudio::Armature * _armature;
    };
}

#endif
