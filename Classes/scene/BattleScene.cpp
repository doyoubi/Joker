#include "BattleScene.h"
#include "role/Role.h"

namespace joker
{

    // BattleScene
    bool BattleScene::init()
    {
        if (!Scene::init()) return false;

        Role::loadAnimationSource();

        auto battleLayer = BattleLayer::create();
        addChild(battleLayer);
    }

    // BattleLayer
    bool BattleLayer::init()
    {
        if (!Layer::init()) return false;

        auto joker = Role::create("joker");
        joker->setPosition(200, 200);
        addChild(joker);
    }

}
