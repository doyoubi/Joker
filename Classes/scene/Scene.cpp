#include "cocostudio/CocoStudio.h"
#include "ui/UIButton.h"

#include "Scene.h"
#include "BattleScene.h"
#include "LoadingCurtain.h"
#include "utils/config.h"
#include "utils/debug.h"

namespace joker
{
    // LoadingScene
    bool LoadingScene::init()
    {
        if (!Scene::init()) return false;

        auto size = Director::getInstance()->getVisibleSize();
        Vec2 center(size.width / 2.0f, size.height / 2.0f);
        _loadingCurtain = LoadingCurtain::create();
        _loadingCurtain->setPosition(center);
        addChild(_loadingCurtain, 5);
        _loadingCurtain->loading();

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
        using namespace cocostudio;
        static bool loadOnceTag = false;
        if (!loadOnceTag)
        {
            loadBlackImg();
            loadOnceTag = true;
            ArmatureDataManager::getInstance()->addArmatureFileInfo("UI/title/title.ExportJson");
        }

        using namespace cocos2d::ui;
        static float startX = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "enterButtonPositionX" });
        static float startY = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "enterButtonPositionY" });
        static float instructionX = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "instructionButtonPositionX" });
        static float instructionY = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "instructionButtonPositionY" });

        auto size = Director::getInstance()->getVisibleSize();
        Vec2 center(size.width / 2.0f, size.height / 2.0f);

        auto start = Button::create(
            "UI/EnterGameButton.png", "UI/EnterGameButton2.png", "UI/EnterGameButton2.png");
        auto instruction = Button::create(
            "UI/EnterInstructionButton.png", "UI/EnterInstructionButton2.png", "UI/EnterInstructionButton2.png");
        start->setPosition(Vec2(size.width / 2.0f + startX, size.height / 2.0f + startY));
        instruction->setPosition(Vec2(size.width / 2.0f + instructionX, size.height / 2.0f + instructionY));
        addChild(start);
        addChild(instruction);
        start->addTouchEventListener([this, start](Ref*, Widget::TouchEventType){
            _loadingCurtain->fallDown();
            start->setTouchEnabled(false);
        });
        instruction->addTouchEventListener([instruction](Ref*, Widget::TouchEventType){
            Director::getInstance()->replaceScene(InstructionScene::create());
            instruction->setTouchEnabled(false);
        });

        static string bgAnim = Config::getInstance().getStringValue({ "UI", "EnterScene", "animation" });
        static string bgMove = Config::getInstance().getStringValue({ "UI", "EnterScene", "movement" });
        static float bgScale = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "backgroundScale" });
        static float bgX = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "backgroundPositionX" });
        static float bgY = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "backgroundPositionY" });
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(bgAnim) != nullptr,
            "missing animation: " + bgAnim);
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(bgAnim)->getMovement(bgMove) != nullptr,
            "missing movement: " + bgMove);
        auto bg = Armature::create(bgAnim);
        bg->getAnimation()->play(bgMove);
        bg->setScale(bgScale);
        bg->setPosition(Vec2(size.width / 2.0f + bgX, size.height / 2.0f + bgY));
        addChild(bg, -1);

        auto backgroundPic = Sprite::create("UI/EnterGameBG.png");
        backgroundPic->setScale(std::max(size.width / backgroundPic->getContentSize().width,
            size.height / backgroundPic->getContentSize().height));
        backgroundPic->setPosition(size.width / 2.0f, size.height / 2.0f);
        addChild(backgroundPic, -2);

        _loadingCurtain = LoadingCurtain::create();
        _loadingCurtain->setPosition(center);
        addChild(_loadingCurtain, 5);

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
        auto label = Label::createWithSystemFont(instructionText, "Arial", 24);
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

    // GameOverScene
    bool GameOverScene::init()
    {
        if (!Scene::init()) return false;

        auto size = Director::getInstance()->getVisibleSize();
        Vec2 center(size.width / 2.0f, size.height / 2.0f);

        using namespace cocos2d::ui;
        auto btn = Button::create("UI/ScorePanelExit.png", "UI/ScorePanelExit.png", "UI/ScorePanelExit.png");
        btn->setPosition(center);
        addChild(btn);
        btn->addTouchEventListener([](Ref*, Widget::TouchEventType){
            Director::getInstance()->replaceScene(EnterGameScene::create());
        });

        auto scoreLabel = Label::create(std::to_string(_score), "fonts/Marker Felt.ttf", 20);
        scoreLabel->setPosition(center);
        addChild(scoreLabel, 2);
        return true;
    }

}
