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
            addArmatureFileInfo = true;
        }
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("cake") != nullptr,
            "missing animation: cake");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("curtain") != nullptr,
            "missing animation: curtain");
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("stage") != nullptr,
            "missing animation: stage");
        _cake = Armature::create("cake");
        _curtain = Armature::create("curtain");
        _stage = Armature::create("stage");
        _cake->setLocalZOrder(-3);
        _stage->setLocalZOrder(-2);
        _curtain->setLocalZOrder(-1);

        Armature * arms[] = { _cake, _stage, _curtain };
        for (auto a : arms)
        {
            DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("cake")->getMovement("show") != nullptr,
                "missing animation: cake");
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
