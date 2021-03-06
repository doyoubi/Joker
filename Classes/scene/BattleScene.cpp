#include <iostream>

#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ui/UIHelper.h"
#include "ui/UIButton.h"

#include "BattleScene.h"
#include "RoleSprite.h"
#include "gameplay/BattleDirector.h"
#include "utils/debug.h"
#include "SimplePhysics/PhysicsWorld.h"
#include "utils/config.h"
#include "LoadingCurtain.h"
#include "Scene.h"
#include "utils/AnimationSprite.h"

namespace joker
{

    // BattleScene
    bool BattleScene::init()
    {
        if (!Scene::init()) return false;

        _battleLayer = BattleLayer::create();
        _battleLayer->retain();
        _battleLayer->setEnterAnimFinishCallback([this](){ getBattleDirector()->startBattle(); });

        _promptBar = PromptBar::create();
        addChild(_promptBar, 4);

        // _battleDirector should be init after _promptBar, battleLayer and before uiLayer
        _battleDirector = unique_ptr<BattleDirector>(new BattleDirector(this));

        auto uiLayer = BattleUILayer::create(getBattleDirector());
        uiLayer->setName("BattleUILayer");
        addChild(uiLayer, 4);

        float hpBarX = Config::getInstance().getDoubleValue({ "UI", "HP", "positionX" });
        float hpBarY = Config::getInstance().getDoubleValue({ "UI", "HP", "positionY" });
        _hpBar = HpBar::create();
        _hpBar->setPosition(hpBarX, hpBarY);
        addChild(_hpBar, 4);

        float scoreX = Config::getInstance().getDoubleValue({ "UI", "score", "positionX" });
        float scoreY = Config::getInstance().getDoubleValue({ "UI", "score", "positionY" });
        _scoreDisplayer = ScoreDisplayer::create();
        _scoreDisplayer->setPosition(scoreX, scoreY);
        addChild(_scoreDisplayer, 4);

        auto size = Director::getInstance()->getVisibleSize();
        Vec2 center(size.width / 2.0f, size.height / 2.0f);
        auto loadingCurtain = LoadingCurtain::create();
        loadingCurtain->setPosition(center);
        addChild(loadingCurtain, 5);
        loadingCurtain->drawUp();
        loadingCurtain->setDrawUpEndCallback([this](){
            BattleScene * scene = dynamic_cast<BattleScene*>(getScene());
            CHECKNULL(scene);
            addChild(getBattleLayer());
        });

        return true;
    }

    void BattleScene::onExit()
    {
        Scene::onExit();
        _battleLayer->release();
    }

    BattleLayer * BattleScene::getBattleLayer()
    {
        CHECKNULL(_battleLayer);
        return _battleLayer;
    }

    BattleUILayer * BattleScene::getUIBattleLayer()
    {
        BattleUILayer * ret = dynamic_cast<BattleUILayer*>(getChildByName("BattleUILayer"));
        CHECKNULL(ret);
        return ret;
    }

    void BattleScene::showResult(int score)
    {
        auto size = Director::getInstance()->getVisibleSize();
        Vec2 center(size.width / 2.0f, size.height / 2.0f);
        auto curtain = GameOverAnim::create();
        curtain->setPosition(center);
        addChild(curtain, 5);
        curtain->show(score);
        getBattleLayer()->darken();
    }

    void BattleScene::endBattle()
    {
        getPromptBar()->clearPromptSprite();
        getBattleLayer()->endBattle();
    }

    // BattleLayer

    // SpikesSprite 
    class SpikesSprite : public cocos2d::Node
    {
    public:
        CREATE_FUNC(SpikesSprite);
        void attack();
        void arise();
    private:
        bool init() override;
        Vector<AnimationSprite*> _spikes;
    };

    bool BattleLayer::init()
    {
        if (!Layer::init()) return false;

        _size.width = Config::getInstance().getDoubleValue({ "BattleStage", "width" });
        _size.height = Config::getInstance().getDoubleValue({ "BattleStage", "height" });

        schedule(schedule_selector(BattleLayer::updateBackground));

        _battleStage = BattleStage::create();
        float dx = Config::getInstance().getDoubleValue({ "BattleStage", "positionX" });
        float dy = Config::getInstance().getDoubleValue({ "BattleStage", "positionY" });
        _battleStage->setPosition(_size.width/2.0f + dx, _size.height/2.0f + dy);
        _battleStage->enter();
        addChild(_battleStage, -1);

        auto curtain = Curtain::create();
        static float curtainX = Config::getInstance().getDoubleValue({ "BattleStage", "curtainX" });
        static float curtainY = Config::getInstance().getDoubleValue({ "BattleStage", "curtainY" });
        curtain->setPosition(_size.width / 2.0f + curtainX, _size.height / 2.0f + curtainY);
        addChild(curtain, 5);

        _spikes = SpikesSprite::create();
        _spikes->setVisible(false);
        addChild(_spikes, 1);

        _cakes = LayeringCakes::create(getSize());
        _cakes->setPosition(0, getSize().height / 2.0f);
        addChild(_cakes, -2);

        string backgroundPic = Config::getInstance().getStringValue({ "backgroundPicture" });
        auto background = Sprite::create(backgroundPic);
        background->setPosition(_size.width / 2.0f, _size.height / 2.0f);
        addChild(background, -3);

        return true;
    }

    BattleLayer::~BattleLayer() {
        unschedule(schedule_selector(BattleLayer::updateBackground));
    }

    void BattleLayer::setEnterAnimFinishCallback(std::function<void(void)> callback)
    {
        _battleStage->setEnterAnimFinishCallback(callback);
    }

    RoleSprite * BattleLayer::addEnemySprite(const Vec2 & position)
    {
        RoleSprite * enemy = RoleSprite::create(
            Config::getInstance().getStringValue({ "RoleProperty", "enemy", "animationName" }),
            Config::getInstance().getStringValue({ "RoleProperty", "enemy", "exportJsonFile" }),
            Config::getInstance().getStringValue({ "RoleProperty", "enemy", "animationDirection" }));
        enemy->setPosition(position);
        const static float localZ = Config::getInstance().getDoubleValue({ "RoleProperty", "enemy", "localZ" });
        enemy->setLocalZOrder(localZ);
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
        _player = RoleSprite::create(
            Config::getInstance().getStringValue({"RoleProperty", "player", "animationName"}),
            Config::getInstance().getStringValue({ "RoleProperty", "player", "exportJsonFile" }),
            Config::getInstance().getStringValue({ "RoleProperty", "player", "animationDirection" }));
        const static float localZ = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "localZ" });
        _player->setLocalZOrder(localZ);
        addChild(_player);
        _player->deadCallback = [this](){
            BattleScene * scene = dynamic_cast<BattleScene*>(this->getParent());
            CHECKNULL(scene);
            scene->showResult(scene->getBattleDirector()->getBattleJudge().getScore());
        };
        return _player;
    }

    RoleSprite * BattleLayer::addBombSprite(const cocos2d::Vec2 & position)
    {
        auto bomb = RoleSprite::create(
            Config::getInstance().getStringValue({ "RoleProperty", "bomb", "animationName" }),
            Config::getInstance().getStringValue({ "RoleProperty", "bomb", "exportJsonFile" }),
            Config::getInstance().getStringValue({ "RoleProperty", "bomb", "animationDirection" }));
        const static float localZ = Config::getInstance().getDoubleValue({ "RoleProperty", "bomb", "localZ" });
        bomb->setLocalZOrder(localZ);
        addChild(bomb);
        return bomb;
    }

    void BattleLayer::updateBackground(float dt) {
        Size visibleSize = Director::getInstance()->getVisibleSize();

        float bgLeft = 0, x = _player->getPositionX();

        if (x <= visibleSize.width / 2) {
            bgLeft = 0;
        }
        else {
            bgLeft = visibleSize.width / 2 - x;
        }
        if (bgLeft + _size.width < visibleSize.width) {
            bgLeft = visibleSize.width - _size.width;
        }
        float formerBgLeft = getPositionX();
        float delta = bgLeft - formerBgLeft;
        setPosition(formerBgLeft + delta / 10.0f, 0);

        float cameraPosition = visibleSize.width / 2.0f - bgLeft;
        _cakes->updatePosition(Vec2(cameraPosition, visibleSize.height / 2.0f));
    }

    void BattleLayer::endBattle()
    {
        unschedule(schedule_selector(BattleLayer::updateBackground));
        _spikes->setVisible(false);
    }

    void BattleLayer::darken()
    {
        auto black = Sprite::createWithSpriteFrame(
            SpriteFrameCache::getInstance()->getSpriteFrameByName("blackSpriteFrame"));
        auto size = Director::getInstance()->getVisibleSize();
        float x = this->getPositionX();
        black->setPosition(-x + size.width / 2.0f, size.height / 2.0f);  // camera position
        black->setOpacity(0);
        addChild(black, 4);
        auto darkenAnim = FadeTo::create(0.8, 150);
        black->runAction(darkenAnim);
    }

    void BattleLayer::spikeArise(const cocos2d::Vec2 & position)
    {
        static float height = Config::getInstance().getDoubleValue({ "RoleProperty", "spike", "positionY" });
        _spikes->setPosition(position.x, height);
        _spikes->arise();
        _spikes->setVisible(true);
    }

    void BattleLayer::spikeAttack()
    {
        _spikes->attack();
    }

    void SpikesSprite::arise()
    {
        const static string animationName = Config::getInstance().getStringValue({ "RoleProperty", "spike", "animationName" });
        const static string movementName = Config::getInstance().getStringValue({ "animation", "spike", "static" });
        using namespace cocostudio;
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(animationName)->getMovement(movementName) != nullptr,
            "missing animation: " + movementName);
        for (auto spike : _spikes)
            spike->playAnimAction(movementName);
    }

    bool SpikesSprite::init()
    {
        if (!Node::init()) return false;

        this->setAnchorPoint(Vec2(0.5f, 0.5f));
        const static float localZ = Config::getInstance().getDoubleValue({ "RoleProperty", "spike", "localZ" });
        this->setLocalZOrder(localZ);

        const static string animationName = Config::getInstance().getStringValue({ "RoleProperty", "spike", "animationName" });
        const static float width = Config::getInstance().getDoubleValue({"RoleProperty", "spike", "spriteWidth"});
        auto size = Director::getInstance()->getVisibleSize();
        int spikeNum = int(size.width / width);
        for (int i = 0; i < spikeNum; ++i)
        {
            _spikes.pushBack(AnimationSprite::create(animationName, "roleAnimation/spike/spike.ExportJson"));
        }
        
        float leftmost = -(spikeNum * width / 2.0f);
        float posi = leftmost + width / 2.0f;
        for (int i = 0; i < _spikes.size(); ++i)
        {
            _spikes.at(i)->setPositionX(posi + width * i);
            addChild(_spikes.at(i));
        }
        return true;
    }

    void SpikesSprite::attack()
    {
        const static string animationName = Config::getInstance().getStringValue({ "RoleProperty", "spike", "animationName" });
        const static string movementName = Config::getInstance().getStringValue({ "animation", "spike", "attack" });
        for (auto spike : _spikes)
            spike->playAnimAction(movementName);
        DEBUGCHECK(!_spikes.empty(), "array of spike sprites is empty");
        _spikes.at(0)->setActionCompleteCallback("attack", [this](){
            this->setVisible(false);
        });
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

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
        registerKeyBoard(director);
#endif

        using namespace cocostudio;
        using namespace cocos2d::ui;
        auto battleUI = GUIReader::getInstance()->widgetFromJsonFile("UI/battleUI/battleUI.json");
        addChild(battleUI);

        auto leftRun = Helper::seekWidgetByName(battleUI, "leftRun");
        auto rightRun = Helper::seekWidgetByName(battleUI, "rightRun");
        auto attack = Helper::seekWidgetByName(battleUI, "attack");
        auto jump = Helper::seekWidgetByName(battleUI, "jump");

        using namespace std;
        leftRun->addTouchEventListener([&director](Ref*, Widget::TouchEventType touchEvent){
            if (touchEvent == Widget::TouchEventType::BEGAN)
            {
                RoleCommand command(RoleAction::RUN);
                command.add<RoleDirection>("direction", RoleDirection::LEFT);
                director->sendCommand(director->getPlayer(), command);
            }
            else if (touchEvent == Widget::TouchEventType::ENDED
                || touchEvent == Widget::TouchEventType::CANCELED)
            {
                RoleCommand command(RoleAction::STOP);
                command.add<RoleDirection>("direction", RoleDirection::LEFT);
                director->sendCommand(director->getPlayer(), command);
            }
        });

        rightRun->addTouchEventListener([&director](Ref*, Widget::TouchEventType touchEvent){
            if (touchEvent == Widget::TouchEventType::BEGAN)
            {
                RoleCommand command(RoleAction::RUN);
                command.add<RoleDirection>("direction", RoleDirection::RIGHT);
                director->sendCommand(director->getPlayer(), command);
            }
            else if (touchEvent == Widget::TouchEventType::ENDED
                || touchEvent == Widget::TouchEventType::CANCELED)
            {
                RoleCommand command(RoleAction::STOP);
                command.add<RoleDirection>("direction", RoleDirection::RIGHT);
                director->sendCommand(director->getPlayer(), command);
            }
        });

        attack->addTouchEventListener([&director](Ref*, Widget::TouchEventType touchEvent){
            if (touchEvent == Widget::TouchEventType::BEGAN)
                director->tabMetronome();
        });

        jump->addTouchEventListener([&director](Ref*, Widget::TouchEventType touchEvent){
            if (touchEvent == Widget::TouchEventType::BEGAN)
                director->sendCommand(director->getPlayer(), RoleAction::JUMP);
        });

        return true;
    }

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    void BattleUILayer::registerKeyBoard(unique_ptr<BattleDirector> & director)
    {
        auto listener = EventListenerKeyboard::create();

        listener->onKeyPressed = [&director](EventKeyboard::KeyCode keyCode, Event* event){
            if (keyCode == EventKeyboard::KeyCode::KEY_A)
            {
                RoleCommand command(RoleAction::RUN);
                command.add<RoleDirection>("direction", RoleDirection::LEFT);
                director->sendCommand(director->getPlayer(), command);
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_D)
            {
                RoleCommand command(RoleAction::RUN);
                command.add<RoleDirection>("direction", RoleDirection::RIGHT);
                director->sendCommand(director->getPlayer(), command);
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_W)
            {
                director->sendCommand(director->getPlayer(), RoleAction::JUMP);
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_J)
            {
                director->tabMetronome();
            }
        };
        listener->onKeyReleased = [&director](EventKeyboard::KeyCode keyCode, Event* event){
            if (keyCode == EventKeyboard::KeyCode::KEY_A)
            {
                RoleCommand command(RoleAction::STOP);
                command.add<RoleDirection>("direction", RoleDirection::LEFT);
                director->sendCommand(director->getPlayer(), command);
            }
            else if (keyCode == EventKeyboard::KeyCode::KEY_D)
            {
                RoleCommand command(RoleAction::STOP);
                command.add<RoleDirection>("direction", RoleDirection::RIGHT);
                director->sendCommand(director->getPlayer(), command);
            }
        };
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    }
#endif

    // BattleResultPanel
    bool BattleResultPanel::init()
    {
        if (!Node::init()) return false;
        auto background = Sprite::create("UI/ScorePanel.png");
        addChild(background, -1);
        using namespace cocos2d::ui;
        auto btn = Button::create("UI/ScorePanelExit.png", "UI/ScorePanelExit.png", "UI/ScorePanelExit.png");
        btn->setPosition(Vec2(-100, -100));
        addChild(btn);
        btn->addTouchEventListener([](Ref*, Widget::TouchEventType){
            Director::getInstance()->replaceScene(EnterGameScene::create());
        });
        return true;
    }

    void BattleResultPanel::setScore(int score)
    {
        Label * scoreLabel = Label::createWithTTF(std::to_string(score), "fonts/Marker Felt.ttf", 100);
        scoreLabel->setColor(Color3B(255, 0, 0));
        addChild(scoreLabel);
    }



}
