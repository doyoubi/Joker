#ifndef JOKER_BATTLE_SCENE
#define JOKER_BATTLE_SCENE

#include <memory>

#include "cocos2d.h"

#include "gameplay/BattleDirector.h"
#include "sound/SoundManager.h"
#include "PromptBar.h"

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

        BattleLayer * getBattleLayer();
        BattleUILayer * getUIBattleLayer();
        unique_ptr<BattleDirector> & getBattleDirector() { return _battleDirector; }
        SoundManager * getSoundManager() { return &_soundManager; }
        unique_ptr<PromptBar> & getPromptBar() { return _promptBar; }

    private:
        bool init() override;

        unique_ptr<BattleDirector> _battleDirector;
        unique_ptr<PromptBar> _promptBar;
        SoundManager  _soundManager;
    };


    class BattleLayer : public cocos2d::Layer
    {
    public:
        CREATE_FUNC(BattleLayer);
        RoleSprite * getPlayerSprite() { return _player; }
        RoleSprite * addEnemySprite(const cocos2d::Vec2 & position);
        RoleSprite * addPlayerSprite(const cocos2d::Vec2 & position);
        void removeEnemySprite(RoleSprite * enemy);
        vector<RoleSprite*> & getEnemySpriteArray() { return _enemyArray; }

        ~BattleLayer();

    private:
        bool init() override;
        void updateBackgroud(float dt);

        vector<RoleSprite*> _enemyArray;  // weak reference
        RoleSprite * _player = nullptr;   // weak reference
        cocos2d::Sprite * _background = nullptr;  // weak reference
    };


    class BattleUILayer : public cocos2d::Layer
    {
    public:
        static BattleUILayer * create(unique_ptr<BattleDirector> & director);
    private:
        bool init(unique_ptr<BattleDirector> & director);
    };

}

#endif
