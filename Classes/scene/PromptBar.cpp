#include <limits>
#include <vector>

#include "PromptBar.h"
#include "utils/debug.h"
#include "utils/config.h"

namespace joker
{
    const float PromptBar::NOT_INIT_TAG = std::numeric_limits<float>::quiet_NaN();
    std::string PromptBar::backgroundAnimProj;
    std::string PromptBar::mark0AnimProj;
    std::string PromptBar::mark1AnimProj;
    std::string PromptBar::mark2AnimProj;
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
    std::string PromptBar::enemy;
    std::string PromptBar::spike;
    std::string PromptBar::bomb;

    void PromptBar::addSource()
    {
        backgroundAnimProj = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "background", "animationProject" });
        mark0AnimProj = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "mark", "mark0_animationProject" });
        mark1AnimProj = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "mark", "mark1_animationProject" });
        mark2AnimProj = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "mark", "mark2_animationProject" });

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
        { "animation", "PromptBar", "mark", "sat" });
        mPerfect = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "mark", "perfect" });
        mGood = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "mark", "good" });
        mOk = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "mark", "ok" });
        mMiss = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "mark", "miss" });

        bomb = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "MovingObject", "bomb" });
        enemy = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "MovingObject", "enemy" });
        spike = Config::getInstance().getStringValue(
        { "animation", "PromptBar", "MovingObject", "spike" });

        string anims[] = { backgroundAnimProj, mark0AnimProj, mark1AnimProj, mark2AnimProj };

        using namespace cocostudio;
        for (string & anim : anims)
        {
            ArmatureDataManager::getInstance()->addArmatureFileInfo(
                "PromptBar/" + anim + "/" + anim + ".ExportJson");
        }
    }


    PromptBar::PromptBar(Node * parent)
    {
        static float barPositionX = Config::getInstance().getDoubleValue({ "PromptBar", "barPositionX" });
        static float barPositionY = Config::getInstance().getDoubleValue({ "PromptBar", "barPositionY" });
        static float startX = Config::getInstance().getDoubleValue({ "PromptBar", "startX" });
        static float startY = Config::getInstance().getDoubleValue({ "PromptBar", "startY" });
        static float endX = Config::getInstance().getDoubleValue({ "PromptBar", "endX" });
        static float endY = Config::getInstance().getDoubleValue({ "PromptBar", "endY" });

        using namespace cocostudio;
        static bool addArmatureFileInfo = false;
        if (!addArmatureFileInfo)
        {
            addSource();
            addArmatureFileInfo = true;
        }

        string bMovement[] = { bSat, bPerfect, bGood, bOk, bMiss };
        string mMovement[] = { mSat, mPerfect, mGood, mOk, mMiss };
        string anims[] = { backgroundAnimProj, mark0AnimProj, mark1AnimProj, mark2AnimProj };

        for (string & anim : anims)
        {
            DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(anim) != nullptr,
                "missing animation: " + anim);
        }
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

        string marks[] = { mark0AnimProj, mark1AnimProj, mark2AnimProj };
        static float markOffsetX = Config::getInstance().getDoubleValue({ "PromptBar", "markOffsetX" });
        static float markOffsetY = Config::getInstance().getDoubleValue({ "PromptBar", "markOffsetY" });
        for (int i = 0; i < 3; ++i)
        {
            _marks[i] = Armature::create(marks[i]);
            _marks[i]->setPosition(_endPoint + Vec2(markOffsetX, markOffsetY));
            _root->addChild(_marks[i], i+1);
        }
        if (JOKER_DEBUG_ON)
        {
            auto goal = Sprite::create("PromptBar/goal.png");
            CHECKNULL(goal);
            goal->setPosition(_endPoint);
            goal->setAnchorPoint(Vec2(0.5f, 0.5f));
            _root->addChild(goal, 5);
        }
    }

    void PromptBar::addPromptSprite(float moveToTime, PromptSpriteType type)
    {
        using namespace cocostudio;
        Sprite * promptSprite;
        if (type == PromptSpriteType::ATTACK)
            promptSprite = Sprite::create(enemy);
        else if (type == PromptSpriteType::SPIKE)
            promptSprite = Sprite::create(spike);
        else if (type == PromptSpriteType::BOMB)
            promptSprite = Sprite::create(bomb);

        // only for debug
        if (JOKER_DEBUG_ON)
        {
            auto child = Sprite::create("PromptBar/goal.png");
            auto size = promptSprite->getContentSize();
            child->setPosition(size.width / 2.0f, size.height / 2.0f);
            promptSprite->addChild(child);
        }

        CHECKNULL(promptSprite);
        promptSprite->setAnchorPoint(Vec2(0.5, 0.5));
        promptSprite->setPosition(_startPoint);
        _promptSpriteQueue.push(promptSprite);
        _root->addChild(promptSprite, 2);

        static float missEndPointX = Config::getInstance().getDoubleValue({ "PromptBar", "missEndX" });
        DEBUGCHECK(missEndPointX < -30, 
            "missEndX must be nagative and should not too small, "
            "or it will be deleted before we decide whether success or miss");
        float s = std::abs(_startPoint.x - _endPoint.x);
        float t = moveToTime * std::abs(_startPoint.x - missEndPointX) / s;

        cocos2d::ActionInterval * act;
        FiniteTimeAction * moveTo = cocos2d::MoveTo::create(t, _endPoint + Vec2(missEndPointX, 0));
        auto & spriteQueue = _promptSpriteQueue;
        FiniteTimeAction * callback = cocos2d::CallFunc::create([promptSprite, &spriteQueue](){
            DEBUGCHECK(spriteQueue.empty() || promptSprite != spriteQueue.front(), "queue empty or delete a sprite which is still in the queue");
            promptSprite->removeFromParent();
        });
        Vector<FiniteTimeAction*> arr;
        arr.pushBack(moveTo);
        arr.pushBack(callback);
        act = cocos2d::Sequence::create(arr);
        promptSprite->runAction(act);
    }

    void PromptBar::hit(HitResult result)
    {
        using namespace cocostudio;
        DEBUGCHECK(_promptSpriteQueue.size() > 0, "empty queue");
        auto sprite = _promptSpriteQueue.front();
        _promptSpriteQueue.pop();
        string anim =
            result == HitResult::BOMB ? mOk :
            result == HitResult::PERFECT ? mPerfect :
            result == HitResult::GOOD ? mGood :
            result == HitResult::OK ? mOk :
            result == HitResult::MISS ? mMiss : "invalid-anim";
        if (result != HitResult::MISS)
        {
            sprite->removeFromParent();
        }
        string sat = mSat;
        for (auto mark : _marks)
        {
            mark->getAnimation()->play(anim);
            mark->getAnimation()->setMovementEventCallFunc(
                [mark, sat](Armature *armature, MovementEventType movementType, const std::string& movementID){
                if (movementType == MovementEventType::COMPLETE)
                    mark->getAnimation()->play(sat);
            });
        }
        anim =
            result == HitResult::BOMB ? mOk :
            result == HitResult::PERFECT ? mPerfect :
            result == HitResult::GOOD ? mGood :
            result == HitResult::OK ? mOk :
            result == HitResult::MISS ? mMiss : "invalid-anim";
        _barBackground->getAnimation()->play(anim);
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
