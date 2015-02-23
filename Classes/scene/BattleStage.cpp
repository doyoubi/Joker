#include "BattleStage.h"

#include "utils/debug.h"

namespace joker
{
    using cocostudio::Armature;

    bool BattleStage::init()
    {
        if (!Node::init()) return false;

        using namespace cocostudio;
        static bool addArmatureFileInfo = false;
        if (!addArmatureFileInfo)
        {
            ArmatureDataManager::getInstance()->addArmatureFileInfo("background/cake/cake.ExportJson");
            ArmatureDataManager::getInstance()->addArmatureFileInfo("background/curtain/curtain.ExportJson");
            ArmatureDataManager::getInstance()->addArmatureFileInfo("background/stage/stage.ExportJson");
            ArmatureDataManager::getInstance()->addArmatureFileInfo("background/BG/BG.ExportJson");
            addArmatureFileInfo = true;
        }
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("cake") != nullptr,
            "missing animation: cake");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("curtain") != nullptr,
            "missing animation: curtain");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("stage") != nullptr,
            "missing animation: stage");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("BG") != nullptr,
            "missing animation: stage");
        _background = Armature::create("BG");
        _cake = Armature::create("cake");
        _curtain = Armature::create("curtain");
        _stage = Armature::create("stage");
        _background->setLocalZOrder(-4);
        _cake->setLocalZOrder(-3);
        _stage->setLocalZOrder(-2);
        _curtain->setLocalZOrder(-1);

        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("BG")->getMovement("static") != nullptr,
            "missing animation movement: static");
        _background->getAnimation()->play("static");
        addChild(_background);

        Armature * arms[] = { _cake, _stage, _curtain };
        string animationData[] = { "cake", "stage", "curtain" };
        for (int i = 0; i < sizeof(arms) / sizeof(Armature *); ++i)
        {
            auto a = arms[i];
            string & animData = animationData[i];
            DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(animData)->getMovement("show") != nullptr,
                "missing animation movement: " + animData);
            addChild(a);
            a->getAnimation()->play("show");
            a->pause();
        }
        _curtain->getAnimation()->setMovementEventCallFunc(
            [this](Armature *armature, MovementEventType movementType, const std::string& movementID){
            if (movementType == MovementEventType::COMPLETE && movementID == "show")
            {
                _curtain->getAnimation()->play("static");
            }
        });
        return true;
    }

    void BattleStage::enter()
    {
        _stage->resume();
        _curtain->resume();
        _stage->resume();
    }

    void BattleStage::shake()
    {
        _stage->getAnimation()->play("shake");
    }

    void BattleStage::quake()
    {
        _stage->getAnimation()->play("quake");
        _cake->getAnimation()->play("quake");
    }

}
