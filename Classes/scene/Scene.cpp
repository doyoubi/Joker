#include "ui/UIButton.h"
#include "SimpleAudioEngine.h"

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
            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
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
        auto bg = AnimationSprite::create(bgAnim, "UI/title/title.ExportJson");
        bg->playAnimAction(bgMove);
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

        if (!CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
        {
            static string bgm = Config::getInstance().getStringValue({ "MusicScript", "backgroundMusic" });
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(bgm.c_str());
        }
    }

    // InstructionScene
    bool InstructionScene::init()
    {
        if (!Scene::init()) return false;

        auto size = Director::getInstance()->getVisibleSize();

        _rules = AnimationSprite::create("rules", "rules/rules.ExportJson");
        CHECKNULL(_rules);
        addChild(_rules);
        _rules->setPosition(size.width / 2.0f, size.height / 2.0f);
        _rules->playAnimAction("show");

        using namespace cocos2d::ui;
        auto back = Button::create("UI/back.png", "UI/back.png", "UI/back.png");
        static float backX = Config::getInstance().getDoubleValue({ "UI", "InstructionScene", "backButtonPositionX" });
        static float backY = Config::getInstance().getDoubleValue({ "UI", "InstructionScene", "backButtonPositionY" });
        back->setPosition(Vec2(size.width / 2.0f + backX, size.height / 2.0f + backY));
        addChild(back);
        back->addTouchEventListener([](Ref*, Widget::TouchEventType){
            Director::getInstance()->replaceScene(EnterGameScene::create());
        });

        auto next = Button::create("UI/next.png", "UI/next.png", "UI/next.png");
        static float nextX = Config::getInstance().getDoubleValue({ "UI", "InstructionScene", "nextButtonPositionX" });
        static float nextY = Config::getInstance().getDoubleValue({ "UI", "InstructionScene", "nextButtonPositionY" });
        next->setPosition(Vec2(size.width / 2.0f + nextX, size.height / 2.0f + nextY));
        addChild(next);
        next->addTouchEventListener([this](Ref*, Widget::TouchEventType){
            if (!_rules->isComplete())
                return;
            static bool firstPage = true;
            if (firstPage)
                _rules->playAnimAction("pg2nothing");
            else
                _rules->playAnimAction("page2out");
            firstPage = !firstPage;
        });
        _rules->setActionCompleteCallback("page2out", [this](){
            _rules->playAnimAction("show");
        });

        static float cprX = Config::getInstance().getDoubleValue({ "UI", "InstructionScene", "copyrightX" });
        static float cprY = Config::getInstance().getDoubleValue({ "UI", "InstructionScene", "copyrightY" });
        auto copyright = Sprite::create("UI/copyright.png");
        copyright->setScale(0.5);
        addChild(copyright);
        copyright->setPosition(Vec2(size.width / 2.0f + cprX, size.height / 2.0f + cprY));

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

        _curtain = AnimationSprite::create("gameover", "UI/gameover/gameover.ExportJson");
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
        _curtain->playAnimAction("show");
        _curtain->setActionCompleteCallback("show", [this, score](){
            auto scene = GameOverScene::create();
            scene->setScore(score);
            Director::getInstance()->replaceScene(scene);
        });
    }

    void GameOverAnim::move()
    {
        _curtain->playAnimAction("move");
        using namespace cocostudio;
        _curtain->setActionCompleteCallback("move", [this](){ this->showScore(); });
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
        _curtain->playAnimAction("over");
        removeChildByName("returnButton");
        removeChildByName("scoreLabel");
        _curtain->setActionCompleteCallback("over", [](){
            Director::getInstance()->replaceScene(EnterGameScene::create());
        });
    }

}
