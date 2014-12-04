#include <iostream>

#include "cocostudio/CocoStudio.h"
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

        getSoundManager()->loadSound("badapple", "music/badapple.wav");
        getSoundManager()->loadSound("hit", "music/knock.wav");
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

        _background = Sprite::create("background/background.png");
        _background->setAnchorPoint(Point(0,0));

        addChild(_background, -1);

        schedule(schedule_selector(BattleLayer::updateBackgroud));

        SimplePhysics::setWorldWidth(_background->getContentSize().width);

        return true;
    }

    BattleLayer::~BattleLayer() {
        unschedule(schedule_selector(BattleLayer::updateBackgroud));
    }

    Role * BattleLayer::addEnemy(const Vec2 & position)
    {
        Role * enemy = Role::create("enemy");
        enemy->setSpeed(100, 10);
        enemy->setPosition(position);
        _enemyArray.push_back(enemy);
        addChild(enemy);
        return enemy;
    }

    void BattleLayer::removeEnemy(Role * enemy)
    {
        auto it = std::find(std::begin(_enemyArray), std::end(_enemyArray), enemy);
        DEBUGCHECK(it != std::end(_enemyArray), "enemy not exist!");
        _enemyArray.erase(it);

        removeChild(enemy, true);
    }

    Role * BattleLayer::addPlayer(const cocos2d::Vec2 & position)
    {
        DEBUGCHECK(_player == nullptr, "player already exist");
        _player = Role::create("joker");
        _player->setPosition(200, 200);
        addChild(_player);
        return _player;
    }

    void BattleLayer::updateBackgroud(float dt) {
        Size visibleSize = Director::getInstance()->getVisibleSize();

        float bgLeft = 0, x = _player->getSimplePhysics()->getX();

        if (x <= visibleSize.width / 2) {
            bgLeft = 0;
        }
        else {
            bgLeft = visibleSize.width / 2 - x;
        }
        if (bgLeft + _background->getContentSize().width < visibleSize.width) {
            bgLeft = visibleSize.width - _background->getContentSize().width;
        }
        setPosition(bgLeft,0);
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
        auto attack = Helper::seekWidgetByName(battleUI, "attack");
        auto jump = Helper::seekWidgetByName(battleUI, "jump");
        auto rhythmStart = Helper::seekWidgetByName(battleUI, "rhythmStart");

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

        attack->addTouchEventListener([&director](Ref*, Widget::TouchEventType touchEvent){
            if (touchEvent == Widget::TouchEventType::BEGAN)
                director->tagMetronome();
        });

        jump->addTouchEventListener([&director](Ref*, Widget::TouchEventType touchEvent){
            if (touchEvent == Widget::TouchEventType::BEGAN)
                director->sendCommand(director->getPlayer(), RoleAction::JUMP);
        });

        rhythmStart->addTouchEventListener([&director](Ref*, Widget::TouchEventType touchEvent){
            director->restartMetronome();
        });

        return true;
    }


}
