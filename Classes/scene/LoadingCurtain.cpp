#include "cocostudio/CocoStudio.h"

#include "LoadingCurtain.h"
#include "utils/config.h"
#include "utils/debug.h"
#include "utils/GlobalValue.h"
#include "BattleScene.h"
#include "Scene.h"

namespace joker
{

    bool LoadingCurtain::init()
    {
        if (!Node::init()) return false;

        _loadingCurtain = AnimationSprite::create("loading", "UI/loading/loading.ExportJson");
        addChild(_loadingCurtain);
        _loadingCurtain->setActionCompleteCallback("show", [](){
            Director::getInstance()->replaceScene(LoadingScene::create());
        });
        static float curtainX = Config::getInstance().getDoubleValue({ "UI", "LoadingScene", "curtainX" });
        static float curtainY = Config::getInstance().getDoubleValue({ "UI", "LoadingScene", "curtainY" });
        static float curtainScale = Config::getInstance().getDoubleValue({ "UI", "LoadingScene", "curtainScale" });
        _loadingCurtain->setPosition(curtainX, curtainY);
        auto size = Director::getInstance()->getVisibleSize();
        _loadingCurtain->setScale(
            curtainScale * size.width / DesignSizeWidth, curtainScale * size.height / DesignSizeHeight);

        return true;
    }

    void LoadingCurtain::fallDown()
    {
        _loadingCurtain->playAnimAction("show");
    }

    void LoadingCurtain::drawUp()
    {
        _loadingCurtain->playAnimAction("over");
    }

    void LoadingCurtain::loading()
    {
        _loadingCurtain->playAnimAction("loading");
    }

    void LoadingCurtain::setDrawUpEndCallback(DrawUpEndCallback callback)
    {
        _loadingCurtain->setActionCompleteCallback("over", callback);
    }

}
