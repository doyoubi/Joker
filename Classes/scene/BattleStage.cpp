#include "BattleStage.h"

#include "utils/debug.h"
#include "utils/config.h"

namespace joker
{
    using cocostudio::Armature;

    // BattleStage
    bool BattleStage::init()
    {
        if (!Node::init()) return false;

        using namespace cocostudio;
        static bool addArmatureFileInfo = false;
        if (!addArmatureFileInfo)
        {
            ArmatureDataManager::getInstance()->addArmatureFileInfo("background/stage/stage.ExportJson");
            addArmatureFileInfo = true;
        }
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("stage") != nullptr,
            "missing animation: stage");
        _stage = Armature::create("stage");

        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("stage")->getMovement("show") != nullptr,
            "missing animation movement: show");
        addChild(_stage);
        _stage->getAnimation()->play("show");
        _stage->pause();

        return true;
    }

    void BattleStage::enter()
    {
        _stage->resume();
    }

    void BattleStage::shake()
    {
        _stage->getAnimation()->play("shake");
    }

    void BattleStage::quake()
    {
        _stage->getAnimation()->play("quake");
    }

    void BattleStage::setEnterAnimFinishCallback(std::function<void(void)> callback)
    {
        using namespace cocostudio;
        _stage->getAnimation()->setMovementEventCallFunc(
            [this, callback](Armature *armature, MovementEventType movementType, const std::string& movementID){
            if (movementType == MovementEventType::COMPLETE && movementID == "show")
            {
                callback();
            }
        });
    }

    // Curtain
    void Curtain::enter()
    {
        _curtain->resume();
    }

    bool Curtain::init()
    {
        if (!Node::init()) return false;

        using namespace cocostudio;
        static bool addArmatureFileInfo = false;
        if (!addArmatureFileInfo)
        {
            ArmatureDataManager::getInstance()->addArmatureFileInfo("background/curtain/curtain.ExportJson");
            addArmatureFileInfo = true;
        }
        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("curtain") != nullptr,
            "missing animation: curtain");
        _curtain = Armature::create("curtain");

        DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData("stage")->getMovement("show") != nullptr,
            "missing animation movement: show");
        addChild(_curtain);
        _curtain->getAnimation()->play("show");
        _curtain->pause();
        _curtain->getAnimation()->setMovementEventCallFunc(
            [this](Armature *armature, MovementEventType movementType, const std::string& movementID){
            if (movementType == MovementEventType::COMPLETE && movementID == "show")
            {
                _curtain->getAnimation()->play("static");
            }
        });
        return true;
    }

    // LayeringCakes
    LayeringCakes * LayeringCakes::create(Size stageSize)
    {
        LayeringCakes * ret = new (std::nothrow) LayeringCakes();
        if (ret && ret->init(stageSize))
        {
            ret->autorelease();
        }
        else
        {
            CC_SAFE_DELETE(ret);
        }
        return ret;
    }
    bool LayeringCakes::init(Size stageSize)
    {
        if (!Node::init()) return false;

        _stageSize = stageSize;

        layer1Width = Config::getInstance().getDoubleValue({"LayeringCakes", "layer1Width"});
        layer2Width = Config::getInstance().getDoubleValue({ "LayeringCakes", "layer2Width" });
        const float scale1 = Config::getInstance().getDoubleValue({ "LayeringCakes", "scale1" });
        const float scale2 = Config::getInstance().getDoubleValue({ "LayeringCakes", "scale2" });
        const float gap1 = Config::getInstance().getDoubleValue({ "LayeringCakes", "gap1" });
        const float gap2 = Config::getInstance().getDoubleValue({ "LayeringCakes", "gap2" });
        const string layer1Anim = Config::getInstance().getStringValue({ "LayeringCakes", "layer1Anim" });
        const string layer2Anim = Config::getInstance().getStringValue({ "LayeringCakes", "layer2Anim" });
        const float offsetY1 = Config::getInstance().getDoubleValue({ "LayeringCakes", "offsetY1" });
        const float offsetY2 = Config::getInstance().getDoubleValue({ "LayeringCakes", "offsetY2" });

        using namespace cocostudio;
        static bool addArmatureFileInfo = false;
        if (!addArmatureFileInfo)
        {
            ArmatureDataManager::getInstance()->addArmatureFileInfo("background/" + layer1Anim + "/" + layer1Anim + ".ExportJson");
            ArmatureDataManager::getInstance()->addArmatureFileInfo("background/" + layer2Anim + "/" + layer2Anim + ".ExportJson");
            addArmatureFileInfo = true;
        }
        auto anims = std::vector<string>({ layer1Anim, layer2Anim });
        for (auto anim : anims)
        {
            DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(anim) != nullptr,
                "missing animation: " + anim);
            DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(anim)->getMovement("show") != nullptr,
                anim + " missing movement: show");
            DEBUGCHECK(ArmatureDataManager::getInstance()->getAnimationData(anim)->getMovement("static") != nullptr,
                anim + " missing movement: static");
        }

        _layer1 = Node::create();
        _layer2 = Node::create();
        _layer1->setPositionY(offsetY1);
        _layer2->setPositionY(offsetY2);
        addChild(_layer1, 1);
        addChild(_layer2, 0);
        for (float i = -layer1Width / 2.0f; i < layer1Width / 2.0f; i += gap1)
        {
            auto n = Armature::create(layer1Anim);
            n->getAnimation()->play("show");
            n->setPosition(i, 0);
            n->setScale(scale1);
            _layer1->addChild(n);
        }
        for (float i = -layer2Width / 2.0f; i < layer2Width / 2.0f; i += gap2)
        {
            auto n = Armature::create(layer2Anim);
            n->getAnimation()->play("show");
            n->setPosition(i, 0);
            n->setScale(scale2);
            _layer2->addChild(n);
        }
        return true;
    }

    float lerp(float a, float b, float weight)
    {
        return (1 - weight) * a + weight * b;
    }

    void LayeringCakes::updatePosition(Vec2 cameraPosition)
    {
        float cameraX = cameraPosition.x;

        float leftmost1 = layer1Width / 2.0f;
        float rightmost1 = _stageSize.width - layer1Width / 2.0f;
        _layer1->setPosition(lerp(leftmost1, rightmost1, cameraPosition.x / _stageSize.width), _layer1->getPositionY());

        float leftmost2 = layer2Width / 2.0f;
        float rightmost2 = _stageSize.width - layer2Width / 2.0f;
        _layer2->setPosition(lerp(leftmost2, rightmost2, cameraPosition.x / _stageSize.width), _layer2->getPositionY());
    }


}
