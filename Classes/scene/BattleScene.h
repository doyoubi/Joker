#ifndef JOKER_BATTLE_SCENE
#define JOKER_BATTLE_SCENE

#include <memory>

#include "cocos2d.h"

#include "gameplay/BattleDirector.h"
#include "sound/SoundManager.h"

namespace joker
{
    using std::unique_ptr;

    class Role;
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

    private:
        bool init() override;

        unique_ptr<BattleDirector> _battleDirector;
        SoundManager  _soundManager;
    };


    class BattleLayer : public cocos2d::Layer
    {
    public:
        CREATE_FUNC(BattleLayer);
        Role * getPlayer() { return _player; }
        Role * addEnemy(const cocos2d::Vec2 & position);
        Role * addPlayer(const cocos2d::Vec2 & position);
        void removeEnemy(Role * enemy);
        vector<Role*> & getEnemyArray() { return _enemyArray; }

        ~BattleLayer();

    private:
        bool init() override;
        void updateBackgroud(float dt);

        vector<Role*> _enemyArray;  // weak reference
        Role * _player = nullptr;   // weak reference
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
