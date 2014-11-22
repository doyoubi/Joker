#include <iostream>

#include "cocostudio\CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ui/UIHelper.h"

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

        auto uiLayer = BattleUILayer::create();
        addChild(uiLayer);
    }

    // BattleLayer
    bool BattleLayer::init()
    {
        if (!Layer::init()) return false;

        auto joker = Role::create("joker");
        joker->setPosition(200, 200);
        addChild(joker);
    }

    // BattleUILayer
    bool BattleUILayer::init()
    {
        if (!Layer::init()) return false;

        using namespace cocostudio;
        using namespace cocos2d::ui;
        auto battleUI = GUIReader::getInstance()->widgetFromJsonFile("battleUI/battleUI.json");
        addChild(battleUI);
        auto leftRun = Helper::seekWidgetByName(battleUI, "leftRun");
        auto rightRun = Helper::seekWidgetByName(battleUI, "rightRun");
        using namespace std;
        leftRun->addTouchEventListener([](Ref*, Widget::TouchEventType){
            cout << "left run" << endl;
        });
        rightRun->addTouchEventListener([](Ref*, Widget::TouchEventType){
            cout << "left run" << endl;
        });
    }

}
