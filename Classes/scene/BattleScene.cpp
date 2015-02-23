#include <iostream>

#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "ui/UIHelper.h"
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
        
        // global init
        Config::setUsable();
        RoleSprite::loadAnimationSource();

        auto battleLayer = BattleLayer::create();
        battleLayer->setName("BattleLayer");
        addChild(battleLayer);

        _promptBar = unique_ptr<PromptBar>(new PromptBar(this));

        // _battleDirector should be init after _promptBar, battleLayer and before uiLayer
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
        _spikes = SpikesSprite::create();
        _spikes->retain();

        schedule(schedule_selector(BattleLayer::updateBackground));

        _battleStage = BattleStage::create();
        _battleStage->setPosition(_size.width/2.0f, _size.height/2.0f);
        //_battleStage->setAnchorPoint(Point(0, 0));
        _battleStage->enter();
        addChild(_battleStage, -1);

        return true;
    }

    BattleLayer::~BattleLayer() {
        _spikes->release();
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
    }

    void BattleLayer::spikeArise(const cocos2d::Vec2 & position)
    {
        static float height = Config::getInstance().getDoubleValue({ "RoleProperty", "spike", "positionY" });
        _spikes->setPosition(position.x, height);
        _spikes->arise();
        addChild(_spikes);
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
                this->removeFromParent();
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
            else if (touchEvent == Widget::TouchEventType::ENDED)
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
            else if (touchEvent == Widget::TouchEventType::ENDED)
            {
                RoleCommand command(RoleAction::STOP);
                command.add<RoleDirection>("direction", RoleDirection::RIGHT);
                director->sendCommand(director->getPlayer(), command);
            }
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
                director->tagMetronome();
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

}
