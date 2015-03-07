#ifndef JOKER_SCENE
#define JOKER_SCENE

#include "cocos2d.h"

namespace joker
{
    class LoadingCurtain;

    class LoadingScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(LoadingScene);
        void update(float dt); // only to load BattleScene
        void onEnter() override;
    private:
        bool init() override;
        LoadingCurtain * _loadingCurtain;
    };

    class EnterGameScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(EnterGameScene);
        void onEnterTransitionDidFinish() override;
        static void loadBlackImg();
    private:
        bool init() override;
        LoadingCurtain * _loadingCurtain;
    };

    class InstructionScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(InstructionScene);
    private:
        bool init() override;
    };
}

#endif
