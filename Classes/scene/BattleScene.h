#ifndef JOKER_BATTLE_SCENE
#define JOKER_BATTLE_SCENE

#include <memory>

#include "cocos2d.h"

#include "gameplay/BattleDirector.h"
#include "PromptBar.h"
#include "BattleStage.h"
#include "HpScoreBar.h"

namespace joker
{
    using std::unique_ptr;

    class RoleSprite;
    class BattleDirector;
    
    class BattleLayer;
    class BattleUILayer;

    class BattleScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(BattleScene);
        void onEnterTransitionDidFinish() override;

        BattleLayer * getBattleLayer();
        BattleUILayer * getUIBattleLayer();
        unique_ptr<BattleDirector> & getBattleDirector() { return _battleDirector; }
        unique_ptr<PromptBar> & getPromptBar() { return _promptBar; }
        HpBar * getHpBar() { return _hpBar; }
        ScoreDisplayer * getScoreDisplayer() { return _scoreDisplayer; }
        void endBattle();
        void showResult(int score);

    private:
        bool init() override;

        unique_ptr<BattleDirector> _battleDirector;
        unique_ptr<PromptBar> _promptBar;
        HpBar * _hpBar; // weak reference
        ScoreDisplayer * _scoreDisplayer; // weak reference
    };

    class SpikesSprite;
    class BattleLayer : public cocos2d::Layer
    {
    public:
        static void loadBlackImg();
        CREATE_FUNC(BattleLayer);
        RoleSprite * getPlayerSprite() { return _player; }
        RoleSprite * addEnemySprite(const cocos2d::Vec2 & position);
        RoleSprite * addPlayerSprite(const cocos2d::Vec2 & position);
        RoleSprite * addBombSprite(const cocos2d::Vec2 & position);
        void spikeArise(const cocos2d::Vec2 & position);
        void spikeAttack();
        void removeEnemySprite(RoleSprite * enemy);
        void removePlayerSprite();
        void shake(){ _battleStage->shake(); }
        void quake(){ _battleStage->quake(); }
        void darken();
        vector<RoleSprite*> & getEnemySpriteArray() { return _enemyArray; }
        Size getSize() { return _size; }

        ~BattleLayer();
        void endBattle();

    private:
        bool init() override;
        void updateBackground(float dt);

        vector<RoleSprite*> _enemyArray;  // weak reference
        RoleSprite * _player = nullptr;   // weak reference
        BattleStage * _battleStage; // weak ref
        Size _size;
        
        SpikesSprite * _spikes; // strong reference, it's retain() by BattleLayer
        LayeringCakes * _cakes;
    };


    class BattleUILayer : public cocos2d::Layer
    {
    public:
        static BattleUILayer * create(unique_ptr<BattleDirector> & director);
    private:
        bool init(unique_ptr<BattleDirector> & director);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
        void registerKeyBoard(unique_ptr<BattleDirector> & director);
#endif
    };

    class BattleResultPanel : public cocos2d::Node
    {
    public:
        CREATE_FUNC(BattleResultPanel);
        void setScore(int score);
    private:
        bool init() override;
    };

    class LoadingScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(LoadingScene);
        void update(float dt); // only to load BattleScene
        void onEnter() override;
    private:
        bool init() override;
    };

    class EnterGameScene : public cocos2d::Scene
    {
    public:
        CREATE_FUNC(EnterGameScene);
    private:
        bool init() override;
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
