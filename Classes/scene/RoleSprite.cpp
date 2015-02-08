#include "RoleSprite.h"
#include "utils/debug.h"

namespace joker
{
    using namespace cocos2d;
    using namespace cocostudio;
    using std::string;

    void RoleSprite::loadAnimationSource()
    {
        ArmatureDataManager::getInstance()->addArmatureFileInfo(
            "roleAnimation/joker/joker0.png",
            "roleAnimation/joker/joker0.plist",
            "roleAnimation/joker/joker.ExportJson"
            );
        ArmatureDataManager::getInstance()->addArmatureFileInfo(
            "roleAnimation/enemy/enemy0.png",
            "roleAnimation/enemy/enemy0.plist",
            "roleAnimation/enemy/enemy.ExportJson"
            );
    }

    RoleSprite * RoleSprite::create(const string & animationName)
    {
        // check if animation has been loaded
        CHECKNULL(ArmatureDataManager::getInstance()->getAnimationData(animationName));
        CHECKNULL(ArmatureDataManager::getInstance()->getAnimationData(animationName)->getMovement("static"));
        CHECKNULL(ArmatureDataManager::getInstance()->getAnimationData(animationName)->getMovement("run"));
        CHECKNULL(ArmatureDataManager::getInstance()->getAnimationData(animationName)->getMovement("slowDown"));
        Armature * armature = Armature::create(animationName);

        RoleSprite * role = new (std::nothrow) RoleSprite(armature);
        if (!role || !role->init())
        {
            CC_SAFE_DELETE(role);
            return nullptr;
        }
        role->autorelease();
        return role;
    }

    RoleSprite::RoleSprite(Armature * armature)
        : _armature(armature)
    {
        CHECKNULL(_armature);
        addChild(_armature);
        _armature->retain();
    }

    void RoleSprite::die()
    {
        getArmature()->getAnimation()->play("attacked");
        getArmature()->getAnimation()->setMovementEventCallFunc(
            [this](Armature *armature, MovementEventType movementType, const std::string& movementID){
            if (movementType == MovementEventType::COMPLETE)
            {
                this->removeFromParent();
            }
        });
    }
}
