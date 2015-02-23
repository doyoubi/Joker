#ifndef JOKER_BATTLE_STAGE
#define JOKER_BATTLE_STAGE

#include "cocos2d.h"
#include "cocostudio/CCArmature.h"

namespace joker
{
    class BattleStage : public cocos2d::Node
    {
    public:
        CREATE_FUNC(BattleStage);
        void enter();
        void shake();
        void quake();
    private:
        bool init() override;
        cocostudio::Armature * _cake;    // weak ref
        cocostudio::Armature * _curtain; // weak ref
        cocostudio::Armature * _stage;   // weak ref
    };
}

#endif
