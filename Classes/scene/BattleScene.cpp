#include <iostream>

#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ui/UIHelper.h"
#include "ui/UIButton.h"
#include "cocostudio/CCArmature.h"

#include "BattleScene.h"
#include "RoleSprite.h"
#include "gameplay/BattleDirector.h"
#include "utils/debug.h"
#include "SimplePhysics/PhysicsWorld.h"
#include "utils/config.h"

namespace joker
{
    // BattleScene
    bool BattleScene::init()
    {
        if (!Scene::init()) return false;

        auto battleLayer = BattleLayer::create();
        battleLayer->setName("BattleLayer");
        addChild(battleLayer);

        _promptBar = unique_ptr<PromptBar>(new PromptBar(this));

        // _battleDirector should be init after _promptBar, battleLayer and before uiLayer
        _battleDirector = unique_ptr<BattleDirector>(new BattleDirector(this));

        auto uiLayer = BattleUILayer::create(getBattleDirector());
        uiLayer->setName("BattleUILayer");
        addChild(uiLayer);

        float hpBarX = Config::getInstance().getDoubleValue({ "UI", "HP", "positionX" });
        float hpBarY = Config::getInstance().getDoubleValue({ "UI", "HP", "positionY" });
        _hpBar = HpBar::create();
        _hpBar->setPosition(hpBarX, hpBarY);
        addChild(_hpBar);

        float scoreX = Config::getInstance().getDoubleValue({ "UI", "score", "positionX" });
        float scoreY = Config::getInstance().getDoubleValue({ "UI", "score", "positionY" });
        _scoreDisplayer = ScoreDisplayer::create();
        _scoreDisplayer->setPosition(scoreX, scoreY);
        addChild(_scoreDisplayer);

        return true;
    }

    void BattleScene::onEnterTransitionDidFinish()
    {
        Scene::onEnterTransitionDidFinish();
        getBattleDirector()->startBattle();
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

    void BattleScene::showResult(int score)
    {
        auto resultPanel = BattleResultPanel::create();
        resultPanel->setScore(score);
        auto size = Director::getInstance()->getVisibleSize();
        resultPanel->setPosition(size.width / 2.0f, size.height / 2.0f);
        addChild(resultPanel);
        getBattleLayer()->darken();
    }

    void BattleScene::endBattle()
    {
        getPromptBar()->clearPromptSprite();
        getBattleLayer()->endBattle();
    }

    // BattleLayer

    // SpikesSprite 
    using cocostudio::Armature;
    class SpikesSprite : public cocos2d::Node
    {
    public:
        CREATE_FUNC(SpikesSprite);
        void attack();
        void arise();
    private:
        bool init() override;
        Vector<Armature*> _spikes;
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

    RoleSprite * BattleLayer::addEnemySprite(const Vec2 & position)
    {
        RoleSprite * enemy = RoleSprite::create(
            Config::getInstance().getStringValue({ "RoleProperty", "enemy", "animationName" }),
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
        darken();
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
        auto darkenAnim = FadeTo::create(2, 150);
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
            spike->getAnimation()->play(movementName);
    }

    bool SpikesSprite::init()
    {
        if (!Node::init()) return false;

        this->setAnchorPoint(Vec2(0.5f, 0.5f));
        const static float localZ = Config::getInstance().getDoubleValue({ "RoleProperty", "spike", "localZ" });
        this->setLocalZOrder(localZ);

        using cocostudio::ArmatureDataManager;
        static bool addArmatureFileInfo = false;
        if (!addArmatureFileInfo)
        {
            ArmatureDataManager::getInstance()->addArmatureFileInfo(
                "roleAnimation/spike/spike0.png",
                "roleAnimation/spike/spike0.plist",
                "roleAnimation/spike/spike.ExportJson"
                );
            addArmatureFileInfo = true;
        }
        const static string animationName = Config::getInstance().getStringValue({ "RoleProperty", "spike", "animationName" });
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(animationName) != nullptr, "missing animation: " + animationName);
        const static float width = Config::getInstance().getDoubleValue({"RoleProperty", "spike", "spriteWidth"});
        auto size = Director::getInstance()->getVisibleSize();
        int spikeNum = int(size.width / width);
        for (int i = 0; i < spikeNum; ++i)
        {
            _spikes.pushBack(Armature::create(animationName));
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
        using namespace cocostudio;
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(animationName)->getMovement(movementName) != nullptr,
            "missing animation: " + movementName);
        for (auto spike : _spikes)
            spike->getAnimation()->play(movementName);
        DEBUGCHECK(!_spikes.empty(), "array of spike sprites is empty");
        _spikes.at(0)->getAnimation()->setMovementEventCallFunc(
            [this](Armature *armature, MovementEventType movementType, const std::string & movementID){
            if (movementType == MovementEventType::COMPLETE)
            {
                this->setVisible(false);
            }
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

        rhythmStart->addTouchEventListener([&director](Ref*, Widget::TouchEventType){
            director->startBattle();
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


    // LoadingScene
    bool LoadingScene::init()
    {
        if (!Scene::init()) return false;

        Label * loadingTxt = Label::createWithTTF("loading...", "fonts/Marker Felt.ttf", 30);
        auto size = Director::getInstance()->getVisibleSize();
        loadingTxt->setPosition(size.width / 2.0f, size.height / 2.0f);
        addChild(loadingTxt);
        return true;
    }

    void LoadingScene::onEnter()
    {
        Scene::onEnter();
        getScheduler()->scheduleUpdate(this, 0, false);
    }

    void LoadingScene::update(float dt)
    {
        getScheduler()->unscheduleUpdate(this);
        auto battleScene = BattleScene::create();
        Director::getInstance()->replaceScene(battleScene);
    }

    // EnterGameScene
    void EnterGameScene::loadBlackImg()
    {
        auto size = Director::getInstance()->getVisibleSize();
        Texture2D * blackTex = new Texture2D();
        struct RGB{ unsigned r, g, b; };
        int width = 32, height = 32;
        vector<unsigned char> rawData(width * height * sizeof(RGB));
        memset(rawData.data(), 0, rawData.size());
        blackTex->initWithData(rawData.data(), width *  height * sizeof(RGB),
            Texture2D::PixelFormat::RGB888, width, height, Size()); // the last size param is not actually used
        Rect rect(0, 0, size.width, size.height);
        auto spriteFrame = SpriteFrame::createWithTexture(blackTex, rect);
        SpriteFrameCache::getInstance()->addSpriteFrame(spriteFrame, "blackSpriteFrame");
    }

    bool EnterGameScene::init()
    {
        if (!Scene::init()) return false;
        static bool loadOnceTag = false;
        if (!loadOnceTag)
        {
            loadBlackImg();
            loadOnceTag = true;
        }

        static float startX = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "enterButtonPositionX" });
        static float startY = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "enterButtonPositionY" });
        static float instructionX = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "instructionButtonPositionX" });
        static float instructionY = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "instructionButtonPositionY" });

        using namespace cocos2d::ui;
        auto size = Director::getInstance()->getVisibleSize();
        auto start = Button::create("UI/EnterGame.png", "UI/EnterGame.png", "UI/EnterGame.png");
        auto instruction = Button::create("UI/EnterInstruction.png", "UI/EnterInstruction.png", "UI/EnterInstruction.png");
        start->setPosition(Vec2(size.width / 2.0f + startX, size.height / 2.0f + startY));
        instruction->setPosition(Vec2(size.width / 2.0f + instructionX, size.height / 2.0f + instructionY));
        addChild(start);
        addChild(instruction);
        start->addTouchEventListener([](Ref*, Widget::TouchEventType){
            Director::getInstance()->replaceScene(LoadingScene::create());
        });
        instruction->addTouchEventListener([](Ref*, Widget::TouchEventType){
            Director::getInstance()->replaceScene(InstructionScene::create());
        });
        return true;
    }

    void EnterGameScene::onEnterTransitionDidFinish()
    {
        Scene::onEnterTransitionDidFinish();

        auto black = Sprite::createWithSpriteFrame(
            SpriteFrameCache::getInstance()->getSpriteFrameByName("blackSpriteFrame"));
        auto size = Director::getInstance()->getVisibleSize();
        black->setPosition(size.width / 2.0f, size.height / 2.0f);  // camera position
        black->setOpacity(255);
        addChild(black, 4);
        auto fadeIn = FadeTo::create(1, 0);
        black->runAction(fadeIn);
    }

    // InstructionScene
    bool InstructionScene::init()
    {
        if (!Scene::init()) return false;

        string instructionText = FileUtils::getInstance()->getStringFromFile("instruction.txt");
        auto label = LabelTTF::create(instructionText, "Arial", 24);
        addChild(label);
        auto size = Director::getInstance()->getVisibleSize();
        label->setPosition(size.width / 2.0f, size.height / 2.0f);

        using namespace cocos2d::ui;
        auto back = Button::create("UI/back.png", "UI/back.png", "UI/back.png");
        static float backX = Config::getInstance().getDoubleValue({ "UI", "InstructionScene", "backButtonPositionX" });
        static float backY = Config::getInstance().getDoubleValue({ "UI", "InstructionScene", "backButtonPositionY" });
        back->setPosition(Vec2(size.width / 2.0f + backX, size.height / 2.0f + backY));
        addChild(back);
        back->addTouchEventListener([](Ref*, Widget::TouchEventType){
            Director::getInstance()->replaceScene(EnterGameScene::create());
        });

        return true;
    }


}
