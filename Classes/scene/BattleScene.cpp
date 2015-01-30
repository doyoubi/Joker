#include <iostream>

#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ui/UIHelper.h"

#include "BattleScene.h"
#include "RoleSprite.h"
#include "gameplay/BattleDirector.h"
#include "utils/debug.h"
#include "SimplePhysics/PhysicsWorld.h"

namespace joker
{
    // BattleScene
    bool BattleScene::init()
    {
        if (!Scene::init()) return false;

        RoleSprite::loadAnimationSource();

        auto battleLayer = BattleLayer::create();
        battleLayer->setName("BattleLayer");
        addChild(battleLayer);

        _battleDirector = unique_ptr<BattleDirector>(new BattleDirector(this));

        auto uiLayer = BattleUILayer::create(getBattleDirector());
        uiLayer->setName("BattleUILayer");
        addChild(uiLayer);

        _promptBar = unique_ptr<PromptBar>(new PromptBar(uiLayer));

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

        auto physicsWorld = joker::PhysicsWorld::getInstance();
        physicsWorld->setWorldWidth(_background->getContentSize().width);
        physicsWorld->setGravity(120.0f);
        physicsWorld->setGroundHeight(200.0f);
        physicsWorld->setResistance(480.0f);

        return true;
    }

    BattleLayer::~BattleLayer() {
        unschedule(schedule_selector(BattleLayer::updateBackgroud));
    }

    RoleSprite * BattleLayer::addEnemySprite(const Vec2 & position)
    {
        RoleSprite * enemy = RoleSprite::create("enemy");
        enemy->setPosition(position);
        _enemyArray.push_back(enemy);
        addChild(enemy);
        return enemy;
    }

    void BattleLayer::removeEnemySprite(RoleSprite * enemy)
    {
        auto it = std::find(std::begin(_enemyArray), std::end(_enemyArray), enemy);
        DEBUGCHECK(it != std::end(_enemyArray), "enemy not exist!");
        _enemyArray.erase(it);

        removeChild(enemy, true);
    }

    RoleSprite * BattleLayer::addPlayerSprite(const cocos2d::Vec2 & position)
    {
        DEBUGCHECK(_player == nullptr, "player already exist");
        _player = RoleSprite::create("joker");
        _player->setPosition(200, 200);
        addChild(_player);
        return _player;
    }

    void BattleLayer::updateBackgroud(float dt) {
        Size visibleSize = Director::getInstance()->getVisibleSize();

        float bgLeft = 0, x = _player->getPositionX();

        if (x <= visibleSize.width / 2) {
            bgLeft = 0;
        }
        else {
            bgLeft = visibleSize.width / 2 - x;
        }
        if (bgLeft + _background->getContentSize().width < visibleSize.width) {
            bgLeft = visibleSize.width - _background->getContentSize().width;
        }
        float formerBgLeft = getPositionX();
        float delta = bgLeft - formerBgLeft;
        setPosition(formerBgLeft + delta / 10.0f, 0);
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
            {
                RoleCommand command(RoleAction::RUN);
                command.add<RoleDirection>("direction", RoleDirection::LEFT);
                director->sendCommand(director->getPlayer(), command);
            }
            else if (touchEvent == Widget::TouchEventType::ENDED)
                director->sendCommand(director->getPlayer(), RoleAction::STOP);
        });

        rightRun->addTouchEventListener([&director](Ref*, Widget::TouchEventType touchEvent){
            if (touchEvent == Widget::TouchEventType::BEGAN)
            {
                RoleCommand command(RoleAction::RUN);
                command.add<RoleDirection>("direction", RoleDirection::RIGHT);
                director->sendCommand(director->getPlayer(), command);
            }
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
