#include "PromptBar.h"
#include "utils/debug.h"

namespace joker
{
    PromptBar::PromptBar(Layer * layer)
    {
        _barBackground = Sprite::create("PromptBar/PromptBarBackground.png");
        CHECKNULL(_barBackground);
        _barBackground->setAnchorPoint(Vec2(0.5f, 0.5f));
        auto size = Director::getInstance()->getVisibleSize();
        _barBackground->setPosition(size.width / 2, 480);
        layer->addChild(_barBackground);

        auto bgSize = _barBackground->getContentSize();
        _startPoint = Vec2(bgSize.width - 70, bgSize.height / 2.0f);
        _endPoint = Vec2(70, bgSize.height / 2.0f);

        _goal = Sprite::create("PromptBar/Goal.png");
        _goal->setAnchorPoint(Vec2(0.5, 0.5));
        _goal->setPosition(_endPoint);
        _barBackground->addChild(_goal);
    }

    void PromptBar::addPromptSprite()
    {
        auto promptSprite = Sprite::create("PromptBar/PromptSprite.png");
        CHECKNULL(promptSprite);
        promptSprite->setAnchorPoint(Vec2(0.5, 0.5));
        auto bgSize = _barBackground->getContentSize();
        promptSprite->setPosition(_startPoint);
        _promptSpriteQueue.push(promptSprite);
        _barBackground->addChild(promptSprite);

        auto moveTo = cocos2d::MoveTo::create(_moveTime, _endPoint - Vec2(20, 0));
        promptSprite->runAction(moveTo);
    }

    void PromptBar::hitSuccess()
    {
        DEBUGCHECK(_promptSpriteQueue.size() > 0, "empty queue");
        auto sprite = _promptSpriteQueue.front();
        _promptSpriteQueue.pop();
        sprite->removeFromParent();

        auto scale = cocos2d::ScaleBy::create(0.1f, 1.5f);
        auto rev = scale->reverse();
        auto seq = cocos2d::Sequence::create(scale, rev, nullptr);
        _goal->runAction(seq);
    }
    
    void PromptBar::hitFail()
    {
    }

    void PromptBar::miss()
    {
        DEBUGCHECK(_promptSpriteQueue.size() > 0, "empty queue");
        auto sprite = _promptSpriteQueue.front();
        _promptSpriteQueue.pop();
        sprite->removeFromParent();
    }

    void PromptBar::rhythm()
    {
    }

    void PromptBar::clearPromptSprite()
    {
        while (!_promptSpriteQueue.empty())
        {
            auto s = _promptSpriteQueue.front();
            _promptSpriteQueue.pop();
            s->removeFromParent();
        }
    }
}
