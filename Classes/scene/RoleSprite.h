#ifndef JOKER_ROLE_SPRITE
#define JOKER_ROLE_SPRITE

#include <string>
#include <functional>

#include "cocos2d.h"

#include "role/RoleEnumType.h"
#include "utils/AnimationSprite.h"

namespace joker
{
    class Role;

    class RoleSprite : public cocos2d::Node, public AnimationSpriteProxy
    {
    public:
        static RoleSprite * create(const std::string & animationName, const std::string & exportJsonFile, std::string animationDirection);

        void setDirection(RoleDirection direction);
        RoleDirection getDirection() const;

        void addRoleSpriteDebug(Role * role);

        void die();
        std::function<void()> deadCallback;

        AnimationSprite * getAnimationSprite() { return _animationSprite; }

    private:
        void removeRoleSpriteDebug();
        RoleSprite(AnimationSprite * animationSprite, std::string animationDirection);
        AnimationSprite * _animationSprite;
        int _animationDirection; // -1 for left, 1 for right
    };

}

#endif
