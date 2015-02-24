#ifndef JOKER_PROMPT_BAR
#define JOKER_PROMPT_BAR

#include <queue>
#include <string>

#include "cocos2d.h"
#include "cocostudio/CCArmature.h"

namespace joker
{
    using cocos2d::Vec2;
    using cocos2d::Layer;
    using cocos2d::Sprite;

    enum class PromptSpriteType
    {
        ATTACK,
        BOMB,
        SPIKE,
    };

    class PromptBar
    {
    public:
        PromptBar(cocos2d::Node * parent);
        void clearPromptSprite();
        // When click start metronome, sometime we will have a double click,
        // and PromptBar will generate two prompt sprite.
        // So, we should clear prompt sprite when metronome start

        void hitSuccess();
        void hitFail();
        void miss();
        void rhythm();
        void addPromptSprite(float moveToTime, PromptSpriteType type);

    private:
        std::queue<cocostudio::Armature*> _promptSpriteQueue;
        cocostudio::Armature * _barBackground;
        cocos2d::Sprite * _goal;

        Vec2 _startPoint;
        Vec2 _endPoint;
        static const float NOT_INIT_TAG;
        float _moveTime = NOT_INIT_TAG;

        static std::string backgroundAnimProj;
        static std::string movingObjAnimProj;
        static std::string bSat, bPerfect, bGood, bOk, bMiss;
        static std::string mSat, mPerfect, mGood, mOk, mMiss;
    };
}

#endif
