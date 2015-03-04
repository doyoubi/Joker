#ifndef JOKER_ROLE_SPRITE
#define JOKER_ROLE_SPRITE

#include <string>
#include <functional>

#include "cocos2d.h"
#include "cocostudio/CCArmature.h"

#include "role/RoleEnumType.h"

namespace joker
{
    class Role;

    class RoleSprite : public cocos2d::Node
    {
    public:
        // load animation project to ArmatureDataManager
        // should be called once before using Role::create
        static void loadAnimationSource();
        static RoleSprite * create(const std::string & animationName, std::string animationDirection);

        void setDirection(RoleDirection direction);
        RoleDirection getDirection() const;

        void addRoleSpriteDebug(Role * role);

        cocostudio::Armature * getArmature() { return _armature; }

        void die();
        std::function<void()> deadCallback;

    private:
        void removeRoleSpriteDebug();
        RoleSprite(cocostudio::Armature * armature, std::string animationDirection);
        cocostudio::Armature * _armature;
        int _animationDirection; // -1 for left, 1 for right
    };

}

#endif
