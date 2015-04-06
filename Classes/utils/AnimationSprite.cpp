#include "AnimationSprite.h"

#include "utils/debug.h"

namespace joker
{
    using cocostudio::Armature;
    using cocostudio::ArmatureDataManager;
    using std::string;

    // AnimationSpriteProxy
    void AnimationSpriteProxy::playAnimAction(const std::string & animActionName)
    {
        return getAnimationSprite()->playAnimAction(animActionName);
    }

    void AnimationSpriteProxy::pause()
    {
        return getAnimationSprite()->pause();
    }

    void AnimationSpriteProxy::resume()
    {
        return getAnimationSprite()->pause();
    }

    void AnimationSpriteProxy::setActionCompleteCallback(
        std::string actionName, std::function<void(void)> callback, bool allAction)
    {
        getAnimationSprite()->setActionCompleteCallback(actionName, std::move(callback), allAction);
    }

    bool AnimationSpriteProxy::isPlaying()
    {
        return getAnimationSprite()->isPlaying();
    }

    bool AnimationSpriteProxy::isComplete()
    {
        return getAnimationSprite()->isComplete();
    }

    // AnimationSprite
    AnimationSprite * AnimationSprite::create(const std::string & animationName, const std::string & exportJsonFile)
    {
        AnimationSprite * ret = new AnimationSprite();
        if (ret && ret->init())
        {
            ret->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(ret);
        }
        if (ArmatureDataManager::getInstance()->getAnimationData(animationName) == nullptr)
        {
            ArmatureDataManager::getInstance()->addArmatureFileInfo(exportJsonFile);
        }
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(animationName) != nullptr,
            "missing animation: " + string(animationName));
        ret->_armature =  Armature::create(animationName);
        CHECKNULL(ret->_armature);
        ret->addChild(ret->_armature);
        return ret;
    }

    void AnimationSprite::playAnimAction(const string & animActionName)
    {
        DEBUGCHECK(_armature->getAnimation()->getAnimationData()->getMovement(animActionName) != nullptr,
            "missing movement: " + string(animActionName));
        _armature->getAnimation()->play(animActionName);
    }

    void AnimationSprite::pause()
    {
        _armature->pause();
    }

    void AnimationSprite::resume()
    {
        _armature->resume();
    }

    bool AnimationSprite::isPlaying()
    {
        return _armature->getAnimation()->isPlaying();
    }

    bool AnimationSprite::isComplete()
    {
        return _armature->getAnimation()->isComplete();
    }


    void AnimationSprite::setActionCompleteCallback(string actionName, std::function<void(void)> callback, bool allAction)
    {
        using namespace cocostudio;
        _armature->getAnimation()->setMovementEventCallFunc(
            [this, actionName, callback, allAction](Armature *armature, MovementEventType movementType, const std::string& movementID){
            if (movementType == MovementEventType::COMPLETE && (allAction || movementID == actionName))
            {
                callback();
            }
        });
    }


}
