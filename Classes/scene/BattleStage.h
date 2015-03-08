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
        void setEnterAnimFinishCallback(std::function<void(void)> callback);
    private:
        bool init() override;
        cocostudio::Armature * _stage;   // weak ref
    };

    class Curtain : public cocos2d::Node
    {
    public:
        CREATE_FUNC(Curtain);
        void enter();
    private:
        bool init() override;
        cocostudio::Armature * _curtain; // weak ref
    };

    class LayeringCakes : public cocos2d::Node
    {
    public:
        static LayeringCakes * create(cocos2d::Size stageSize);
        void updatePosition(cocos2d::Vec2 cameraPosition);
    private:
        bool init(cocos2d::Size stageSize);
        cocos2d::Node * _layer1; // weak ref
        cocos2d::Node * _layer2; // weak ref
        float layer1Width;
        float layer2Width;
        cocos2d::Size _stageSize;
    };
}

#endif
