#ifndef JOKER_SCENE
#define JOKER_SCENE

#include "cocos2d.h"
#include "utils/AnimationSprite.h"

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

    class LogoScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(LogoScene);
        void onEnterTransitionDidFinish() override;
        void fadeOut(float);
    private:
        bool init() override;
        cocos2d::Sprite * _black;
    };

    class EnterGameScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(EnterGameScene);
        void onEnterTransitionDidFinish() override;
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
        AnimationSprite * _rules;
    };

    class GameOverAnim : public cocos2d::Node
    {
    public:
        CREATE_FUNC(GameOverAnim);
        void show(int score);
        void move();
        void over();
        void showScore();
        void setScore(int score) { _score = score; }
    private:
        bool init() override;
        AnimationSprite * _curtain;
        int _score;
    };

    class GameOverScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(GameOverScene);
        void setScore(int score) { _gameoverAnim->setScore(score); }
    private:
        bool init() override;
        GameOverAnim * _gameoverAnim; // weak reference
    };

}

#endif
