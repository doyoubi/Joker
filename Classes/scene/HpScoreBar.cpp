#include "HpScoreBar.h"
#include "utils/config.h"
#include "utils/debug.h"

namespace joker
{
    // HpBar
    bool HpBar::init()
    {
        if (!Node::init()) return false;

        int hp = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "hp" });
        int positionInterval = Config::getInstance().getDoubleValue({ "UI", "HP", "heartInterval" });
        
        for (int i = 0; i < hp; ++i)
        {
            auto heart = Sprite::create("UI/heart.jpg");
            heart->setPositionX(i * positionInterval);
            addChild(heart);
            _hearts.pushBack(heart);
        }
    }

    void HpBar::changeHeartNum(int heartNum)
    {
        if (heartNum > _hearts.size())
        {
            static int hp = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "hp" });
            static int positionInterval = Config::getInstance().getDoubleValue({ "UI", "HP", "heartInterval" });
            DEBUGCHECK(heartNum <= hp, "the number of hearts was already zero");
            for (int i = _hearts.size(); i < heartNum; ++i)
            {
                auto heart = Sprite::create("UI/heart.jpg");
                heart->setPositionX(i * positionInterval);
                addChild(heart);
                _hearts.pushBack(heart);
            }
        }
        else if (heartNum < _hearts.size())
        {
            DEBUGCHECK(!_hearts.empty(), "the number of hearts was already zero");
            while (heartNum < _hearts.size())
            {
                _hearts.back()->removeFromParent();
                _hearts.popBack();
            }
        }
    }

    // ScoreDisplay
    bool ScoreDisplayer::init()
    {
        if (!Node::init()) return false;

        _score = Label::createWithTTF("0", "fonts/Marker Felt.ttf", 30);
        addChild(_score);
    }

    void ScoreDisplayer::changeScore(int score)
    {
        _score->setString(std::to_string(score));
    }
}
