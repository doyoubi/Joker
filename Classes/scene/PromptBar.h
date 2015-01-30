#ifndef JOKER_PROMPT_BAR
#define JOKER_PROMPT_BAR

#include <queue>

#include "cocos2d.h"

namespace joker
{
    using cocos2d::Vec2;
    using cocos2d::Layer;
    using cocos2d::Sprite;

    class PromptBar
    {
    public:
        PromptBar(Layer * layer);
        void clearPromptSprite();
        // When click start metronome, sometime we will have a double click,
        // and PromptBar will generate two prompt sprite.
        // So, we should clear prompt sprite when metronome start

        void hitSuccess();
        void hitFail();
        void miss();
        void rhythm();
        void addPromptSprite();

    private:
        std::queue<Sprite*> _promptSpriteQueue;
        Sprite * _barBackground;
        Sprite * _goal;

        Vec2 _startPoint;
        Vec2 _endPoint;
        const float _moveTime = 0.9f;
    };
}

#endif
