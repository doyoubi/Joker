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
        using namespace cocostudio;
        static bool loadOnceTag = false;
        if (!loadOnceTag)
        {
            loadOnceTag = true;
            ArmatureDataManager::getInstance()->addArmatureFileInfo("UI/loading/loading.ExportJson");
        }

        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("loading") != nullptr,
            "missing animation: loading");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("loading")->getMovement("show") != nullptr,
            "missing movement: show");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("loading")->getMovement("loading") != nullptr,
            "missing movement: loading");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("loading")->getMovement("over") != nullptr,
            "missing movement: over");
        _loadingCurtain = Armature::create("loading");
        addChild(_loadingCurtain);
        _loadingCurtain->getAnimation()->setMovementEventCallFunc(
            [](Armature *armature, MovementEventType movementType, const std::string& movementID){
            if (movementType == MovementEventType::COMPLETE && movementID == "show")
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
        _loadingCurtain->getAnimation()->play("show");
    }

    void LoadingCurtain::drawUp()
    {
        _loadingCurtain->getAnimation()->play("over");
    }

    void LoadingCurtain::loading()
    {
        _loadingCurtain->getAnimation()->play("loading");
    }

    void LoadingCurtain::setDrawUpEndCallback(DrawUpEndCallback callback)
    {
        using namespace cocostudio;
        _loadingCurtain->getAnimation()->setMovementEventCallFunc(
            [callback](Armature *armature, MovementEventType movementType, const std::string& movementID){
            if (movementType == MovementEventType::COMPLETE && movementID == "over")
                callback(armature, movementType, movementID);
        });
    }

}
