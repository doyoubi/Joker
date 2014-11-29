#ifndef JOKER_BATTLE_SCENE
#define JOKER_BATTLE_SCENE

#include <memory>

#include "cocos2d.h"

#include "gameplay/BattleDirector.h"

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

    private:
        bool init() override;

        unique_ptr<BattleDirector> _battleDirector;
    };


    class BattleLayer : public cocos2d::Layer
    {
    public:
        CREATE_FUNC(BattleLayer);
        Role * getPlayer() { return _player; }
        Role * addEnemy(const cocos2d::Vec2 & position);
        vector<Role*> & getEnemyArray() { return _enemyArray; }

    private:
        bool init() override;

        vector<Role*> _enemyArray;  // weak reference
        Role * _player = nullptr;   // weak reference
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
