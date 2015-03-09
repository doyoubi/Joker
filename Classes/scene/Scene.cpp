#include "ui/UIButton.h"

#include "Scene.h"
#include "BattleScene.h"
#include "LoadingCurtain.h"
#include "utils/config.h"
#include "utils/debug.h"
#include "utils/GlobalValue.h"

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
        static float exitX = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "exitButtonX" });
        static float exitY = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "exitButtonY" });
        static float exitScale = Config::getInstance().getDoubleValue({ "UI", "EnterScene", "exitScale" });

        auto size = Director::getInstance()->getVisibleSize();
        Vec2 center(size.width / 2.0f, size.height / 2.0f);

        auto start = Button::create(
            "UI/EnterGameButton.png", "UI/EnterGameButton2.png", "UI/EnterGameButton2.png");
        auto instruction = Button::create(
            "UI/EnterInstructionButton.png", "UI/EnterInstructionButton2.png", "UI/EnterInstructionButton2.png");
        auto exit = Button::create("UI/exit.png", "UI/exit.png", "UI/exit.png");
        start->setPosition(Vec2(center.x + startX, center.y + startY));
        instruction->setPosition(Vec2(center.x + instructionX, center.y + instructionY));
        exit->setPosition(Vec2(center.x + exitX, center.y + exitY));
        exit->setScale(exitScale);
        addChild(start);
        addChild(instruction);
        addChild(exit);
        start->addTouchEventListener([this, start](Ref*, Widget::TouchEventType){
            start->setTouchEnabled(false);
            _loadingCurtain->fallDown();
        });
        instruction->addTouchEventListener([instruction](Ref*, Widget::TouchEventType){
            instruction->setTouchEnabled(false);
            Director::getInstance()->replaceScene(InstructionScene::create());
        });
        exit->addTouchEventListener([this, start](Ref*, Widget::TouchEventType){
            Director::getInstance()->end();
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

        _gameoverAnim = GameOverAnim::create();
        addChild(_gameoverAnim);
        _gameoverAnim->setPosition(center);
        _gameoverAnim->move();

        return true;
    }

    // GameOverAnim
    bool GameOverAnim::init()
    {
        if (!Node::init()) return false;
        using namespace cocostudio;
        static bool loadOnceTag = false;
        if (!loadOnceTag)
        {
            loadOnceTag = true;
            ArmatureDataManager::getInstance()->addArmatureFileInfo("UI/gameover/gameover.ExportJson");
        }
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("gameover") != nullptr,
            "missing animation: gameover");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("gameover")->getMovement("show") != nullptr,
            "missing movement: show");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("gameover")->getMovement("move") != nullptr,
            "missing movement: move");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("gameover")->getMovement("over") != nullptr,
            "missing movement: over");
        _curtain = Armature::create("gameover");
        CHECKNULL(_curtain);
        addChild(_curtain);
        static float scale = Config::getInstance().getDoubleValue({ "UI", "GameOverAnim", "scale" });
        static float x = Config::getInstance().getDoubleValue({ "UI", "GameOverAnim", "x" });
        static float y = Config::getInstance().getDoubleValue({ "UI", "GameOverAnim", "y" });
        _curtain->setPosition(x, y);
        auto size = Director::getInstance()->getWinSize();
        _curtain->setScale(scale * size.width / DesignSizeWidth, scale * size.height / DesignSizeHeight);
        return true;
    }

    void GameOverAnim::show(int score)
    {
        DEBUGCHECK(_curtain->getAnimation()->getAnimationData()->getMovement("show") != nullptr,
            "missing movement: show");
        _curtain->getAnimation()->play("show");
        using namespace cocostudio;
        _curtain->getAnimation()->setMovementEventCallFunc(
            [this, score](Armature *armature, MovementEventType movementType, const std::string& movementID){
            if (movementType == MovementEventType::COMPLETE && movementID == "show")
            {
                auto scene = GameOverScene::create();
                scene->setScore(score);
                Director::getInstance()->replaceScene(scene);
            }
        });
    }

    void GameOverAnim::move()
    {
        DEBUGCHECK(_curtain->getAnimation()->getAnimationData()->getMovement("move") != nullptr,
            "missing movement: move");
        _curtain->getAnimation()->play("move");
        using namespace cocostudio;
        _curtain->getAnimation()->setMovementEventCallFunc(
            [this](Armature *armature, MovementEventType movementType, const std::string& movementID){
            if (movementType == MovementEventType::COMPLETE && movementID == "move")
            {
                this->showScore();
            }
        });
    }

    void GameOverAnim::showScore()
    {
        Vec2 center(0, 0);

        using namespace cocos2d::ui;
        auto btn = Button::create("UI/ReturnToMainPage.png", "UI/ReturnToMainPage2.png", "UI/ReturnToMainPage2.png");
        addChild(btn, 1);
        btn->setName("returnButton");
        btn->addTouchEventListener([](Ref*, Widget::TouchEventType){
            Director::getInstance()->replaceScene(EnterGameScene::create());
        });
        static float btnX = Config::getInstance().getDoubleValue({ "UI", "GameOverScene", "buttonX" });
        static float btnY = Config::getInstance().getDoubleValue({ "UI", "GameOverScene", "buttonY" });
        btn->setPosition(center + Vec2(btnX, btnY));
        btn->addTouchEventListener([this, btn](Ref*, Widget::TouchEventType){
            btn->setTouchEnabled(false);
            this->over();
        });

        static int fontSize = Config::getInstance().getDoubleValue({ "UI", "GameOverScene", "scoreFontSize" });
        auto scoreLabel = Label::create(std::to_string(_score), "fonts/Marker Felt.ttf", fontSize);
        addChild(scoreLabel, 1);
        scoreLabel->setName("scoreLabel");
        static float scoreX = Config::getInstance().getDoubleValue({ "UI", "GameOverScene", "scoreX" });
        static float scoreY = Config::getInstance().getDoubleValue({ "UI", "GameOverScene", "scoreY" });
        scoreLabel->setPosition(center + Vec2(scoreX, scoreY));
        static float scoreR = Config::getInstance().getDoubleValue({ "UI", "GameOverScene", "scoreR" });
        static float scoreG = Config::getInstance().getDoubleValue({ "UI", "GameOverScene", "scoreG" });
        static float scoreB = Config::getInstance().getDoubleValue({ "UI", "GameOverScene", "scoreB" });
        scoreLabel->setColor(Color3B(scoreR, scoreG, scoreB));
    }

    void GameOverAnim::over()
    {
        DEBUGCHECK(_curtain->getAnimation()->getAnimationData()->getMovement("show") != nullptr,
            "missing movement: over");
        _curtain->getAnimation()->play("over");
        removeChildByName("returnButton");
        removeChildByName("scoreLabel");
        using namespace cocostudio;
        _curtain->getAnimation()->setMovementEventCallFunc(
            [this](Armature *armature, MovementEventType movementType, const std::string& movementID){
            if (movementType == MovementEventType::COMPLETE && movementID == "over")
            {
                Director::getInstance()->replaceScene(EnterGameScene::create());
            }
        });
    }

}
