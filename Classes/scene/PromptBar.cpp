#include <limits>
#include <vector>

#include "PromptBar.h"
#include "utils/debug.h"
#include "utils/config.h"

namespace joker
{
    const float PromptBar::NOT_INIT_TAG = std::numeric_limits<float>::quiet_NaN();
    std::string PromptBar::backgroundAnimProj;
    std::string PromptBar::movingObjAnimProj;
    std::string PromptBar::bSat;
    std::string PromptBar::bPerfect;
    std::string PromptBar::bGood;
    std::string PromptBar::bOk;
    std::string PromptBar::bMiss;
    std::string PromptBar::mSat;
    std::string PromptBar::mPerfect;
    std::string PromptBar::mGood;
    std::string PromptBar::mOk;
    std::string PromptBar::mMiss;

    PromptBar::PromptBar(Node * parent)
    {
        static float barPositionX = Config::getInstance().getDoubleValue({ "PromptBar", "barPositionX" });
        static float barPositionY = Config::getInstance().getDoubleValue({ "PromptBar", "barPositionY" });
        static float startX = Config::getInstance().getDoubleValue({ "PromptBar", "startX" });
        static float startY = Config::getInstance().getDoubleValue({ "PromptBar", "startY" });
        static float endX = Config::getInstance().getDoubleValue({ "PromptBar", "endX" });
        static float endY = Config::getInstance().getDoubleValue({ "PromptBar", "endY" });

        backgroundAnimProj = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "background", "animationProject" });
        movingObjAnimProj = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "MovingObject", "animationProject" });

        bSat = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "background", "sat" });
        bPerfect = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "background", "perfect" });
        bGood = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "background", "good" });
        bOk = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "background", "ok" });
        bMiss = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "background", "miss" });

        mSat = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "MovingObject", "sat" });
        mPerfect = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "MovingObject", "perfect" });
        mGood = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "MovingObject", "good" });
        mOk = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "MovingObject", "ok" });
        mMiss = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "MovingObject", "miss" });

        using namespace cocostudio;
        static bool addArmatureFileInfo = false;
        if (!addArmatureFileInfo)
        {
            ArmatureDataManager::getInstance()->addArmatureFileInfo(
                "PromptBar/" + backgroundAnimProj + "/" + backgroundAnimProj + ".ExportJson");
            ArmatureDataManager::getInstance()->addArmatureFileInfo(
                "PromptBar/" + movingObjAnimProj + "/" + movingObjAnimProj + ".ExportJson");
            addArmatureFileInfo = true;
        }
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(backgroundAnimProj) != nullptr,
            "missing animation: " + backgroundAnimProj);
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(movingObjAnimProj) != nullptr,
            "missing animation: " + movingObjAnimProj);
        string bMovement[] = { bSat, bPerfect, bGood, bOk, bMiss };
        string mMovement[] = { mSat, mPerfect, mGood, mOk, mMiss };
        string anims[] = { backgroundAnimProj, movingObjAnimProj };
        for (string & anim : anims)
        {
            for (string & movement : bMovement)
            {
                DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(
                    anim)->getMovement(movement) != nullptr,
                    "missing animation: " + anim + "--" + backgroundAnimProj);
            }
        }

        _root = Node::create();
        parent->addChild(_root);
        auto size = Director::getInstance()->getVisibleSize();
        _root->setPosition(size.width / 2 + barPositionX, barPositionY);

        static float bgX = Config::getInstance().getDoubleValue({ "PromptBar", "backgroundX" });
        static float bgY = Config::getInstance().getDoubleValue({ "PromptBar", "backgroundY" });
        _barBackground = Armature::create(backgroundAnimProj);
        _barBackground->setPosition(bgX, bgY);
        CHECKNULL(_barBackground);
        _barBackground->getAnimation()->play(bSat);
        _root->addChild(_barBackground, 0);

        _startPoint = Vec2(startX, startY);
        _endPoint = Vec2(endX, endY);

        _goal = Sprite::create("PromptBar/Goal.png");
        _goal->setAnchorPoint(Vec2(0.5, 0.5));
        _goal->setPosition(_endPoint);
        _root->addChild(_goal, 1);
    }

    void PromptBar::addPromptSprite(float moveToTime, PromptSpriteType type)
    {
        using namespace cocostudio;
        Armature * promptSprite;
        if (type == PromptSpriteType::ATTACK)
            promptSprite = Armature::create(movingObjAnimProj);
        else if (type == PromptSpriteType::SPIKE)
            promptSprite = Armature::create(movingObjAnimProj);
        else if (type == PromptSpriteType::BOMB)
            promptSprite = Armature::create(movingObjAnimProj);
        CHECKNULL(promptSprite);
        promptSprite->getAnimation()->play(mSat);
        promptSprite->setAnchorPoint(Vec2(0.5, 0.5));
        static float x = Config::getInstance().getDoubleValue({ "PromptBar", "MovingObjectX" });
        static float y = Config::getInstance().getDoubleValue({ "PromptBar", "MovingObjectY" });
        promptSprite->setPosition(_startPoint + Vec2(x, y));
        _promptSpriteQueue.push(promptSprite);
        _root->addChild(promptSprite, 2);

        cocos2d::ActionInterval * act;
        if (type == PromptSpriteType::BOMB)
        {
            FiniteTimeAction * moveTo = cocos2d::MoveTo::create(moveToTime, _endPoint);
            FiniteTimeAction * callback = cocos2d::CallFunc::create([this](){
                hitSuccess();
            });
            Vector<FiniteTimeAction*> arr;
            arr.pushBack(moveTo);
            arr.pushBack(callback);
            act = cocos2d::Sequence::create(arr);
        }
        else
        {
            act = cocos2d::MoveTo::create(moveToTime, _endPoint - Vec2(20, 0));
        }
        promptSprite->runAction(act);
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
        if (_promptSpriteQueue.empty()) return;

        cocostudio::Armature * s = _promptSpriteQueue.front();
        auto scale = cocos2d::ScaleBy::create(0.1f, 1.5f);
        auto rev = scale->reverse();
        auto seq = cocos2d::Sequence::create(scale, rev, nullptr);
        s->runAction(seq);
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
