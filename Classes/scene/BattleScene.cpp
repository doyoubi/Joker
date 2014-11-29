#include <iostream>

#include "cocostudio\CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ui/UIHelper.h"

#include "BattleScene.h"
#include "role/Role.h"
#include "gameplay/BattleDirector.h"
#include "utils/debug.h"

namespace joker
{
    // BattleScene
    bool BattleScene::init()
    {
        if (!Scene::init()) return false;

        Role::loadAnimationSource();

        auto battleLayer = BattleLayer::create();
        battleLayer->setName("BattleLayer");
        addChild(battleLayer);

        _battleDirector = unique_ptr<BattleDirector>(new BattleDirector(this));

        auto uiLayer = BattleUILayer::create(getBattleDirector());
        uiLayer->setName("BattleUILayer");
        addChild(uiLayer);
        return true;
    }

    BattleLayer * BattleScene::getBattleLayer()
    {
        BattleLayer * ret = dynamic_cast<BattleLayer*>(getChildByName("BattleLayer"));
        CHECKNULL(ret);
        return ret;
    }

    BattleUILayer * BattleScene::getUIBattleLayer()
    {
        BattleUILayer * ret = dynamic_cast<BattleUILayer*>(getChildByName("BattleLayer"));
        CHECKNULL(ret);
        return ret;
    }

    // BattleLayer
    bool BattleLayer::init()
    {
        if (!Layer::init()) return false;

        _player = Role::create("joker");
        _player->setPosition(200, 200);
        addChild(_player);

        addEnemy(Vec2(200, 200));
        return true;
    }

    // BattleUILayer
    BattleUILayer * BattleUILayer::create(unique_ptr<BattleDirector> & director)
    {
        BattleUILayer * ret = new (std::nothrow) BattleUILayer();
        if (ret && ret->init(director))
        {
            ret->autorelease();
            return ret;
        }
        else
        {
            CC_SAFE_DELETE(ret);
            return nullptr;
        }
    }

    bool BattleUILayer::init(unique_ptr<BattleDirector> & director)
    {
        CHECKNULL(director);
        if (!Layer::init()) return false;

        using namespace cocostudio;
        using namespace cocos2d::ui;
        auto battleUI = GUIReader::getInstance()->widgetFromJsonFile("battleUI/battleUI.json");
        addChild(battleUI);
        auto leftRun = Helper::seekWidgetByName(battleUI, "leftRun");
        auto rightRun = Helper::seekWidgetByName(battleUI, "rightRun");

        using namespace std;
        leftRun->addTouchEventListener([&director](Ref*, Widget::TouchEventType touchEvent){
            if (touchEvent == Widget::TouchEventType::BEGAN)
                director->sendCommand(director->getPlayer(), RoleAction::LEFT_RUN);
            else if (touchEvent == Widget::TouchEventType::ENDED)
                director->sendCommand(director->getPlayer(), RoleAction::STOP);
        });

        rightRun->addTouchEventListener([&director](Ref*, Widget::TouchEventType touchEvent){
            if (touchEvent == Widget::TouchEventType::BEGAN)
                director->sendCommand(director->getPlayer(), RoleAction::RIGHT_RUN);
            else if (touchEvent == Widget::TouchEventType::ENDED)
                director->sendCommand(director->getPlayer(), RoleAction::STOP);
        });
        return true;
    }

    Role * BattleLayer::addEnemy(const Vec2 & position)
    {
        Role * enemy = Role::create("enemy");
        enemy->setPosition(position);
        _enemyArray.push_back(enemy);
        addChild(enemy);
        return enemy;
    }

}
