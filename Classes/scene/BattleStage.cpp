#include "BattleStage.h"

#include "utils/debug.h"
#include "utils/config.h"

namespace joker
{
    // BattleStage
    bool BattleStage::init()
    {
        if (!Node::init()) return false;

        _stage = AnimationSprite::create("stage", "background/stage/stage.ExportJson");

        addChild(_stage);
        _stage->playAnimAction("show");
        _stage->pause();

        return true;
    }

    void BattleStage::enter()
    {
        _stage->resume();
    }

    void BattleStage::shake()
    {
        _stage->playAnimAction("shake");
    }

    void BattleStage::quake()
    {
        _stage->playAnimAction("quake");
    }

    void BattleStage::setEnterAnimFinishCallback(std::function<void(void)> callback)
    {
        _stage->setActionCompleteCallback("show", callback);
    }

    // Curtain
    void Curtain::enter()
    {
        _curtain->resume();
    }

    bool Curtain::init()
    {
        if (!Node::init()) return false;

        _curtain = AnimationSprite::create("curtain", "background/curtain/curtain.ExportJson");

        addChild(_curtain);
        _curtain->playAnimAction("show");
        _curtain->pause();
        _curtain->setActionCompleteCallback("show", [this](){
            _curtain->playAnimAction("static");
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

        _layer1 = Node::create();
        _layer2 = Node::create();
        _layer1->setPositionY(offsetY1);
        _layer2->setPositionY(offsetY2);
        addChild(_layer1, 1);
        addChild(_layer2, 0);
        for (float i = -layer1Width / 2.0f; i < layer1Width / 2.0f; i += gap1)
        {
            auto n = AnimationSprite::create(layer1Anim, "background/" + layer1Anim + "/" + layer1Anim + ".ExportJson");
            n->playAnimAction("show");
            n->setPosition(i, 0);
            n->setScale(scale1);
            _layer1->addChild(n);
        }
        for (float i = -layer2Width / 2.0f; i < layer2Width / 2.0f; i += gap2)
        {
            auto n = AnimationSprite::create(layer2Anim, "background/" + layer2Anim + "/" + layer2Anim + ".ExportJson");
            n->playAnimAction("show");
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
