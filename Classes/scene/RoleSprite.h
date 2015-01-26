#ifndef JOKER_ROLE_SPRITE
#define JOKER_ROLE_SPRITE

#include <string>

#include "cocos2d.h"
#include "cocostudio/CCArmature.h"

namespace joker
{

    class RoleSprite : public cocos2d::Node
    {
    public:
        // load animation project to ArmatureDataManager
        // should be called once before using Role::create
        static void loadAnimationSource();
        static RoleSprite * create(const std::string & animationName);

        cocostudio::Armature * getArmature() { return _armature; }

    private:
        RoleSprite(cocostudio::Armature * armature);
        cocostudio::Armature * _armature;
    };

}

#endif
