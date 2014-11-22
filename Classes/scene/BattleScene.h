#ifndef JOKER_BATTLE_SCENE
#define JOKER_BATTLE_SCENE

#include "cocos2d.h"

namespace joker
{

    class BattleScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(BattleScene);
    private:
        bool init() override;
    };

    class BattleLayer : public cocos2d::Layer
    {
    public:
        CREATE_FUNC(BattleLayer);
    private:
        bool init() override;
    };

}

#endif
