#include "HpScoreBar.h"
#include "utils/config.h"
#include "utils/debug.h"

namespace joker
{
    // HpBar
    Vec2 heartIndex2Position(int i)
    {
        static int rowSize = Config::getInstance().getDoubleValue({ "UI", "HP", "rowHeartNumber" });
        int positionInterval = Config::getInstance().getDoubleValue({ "UI", "HP", "heartInterval" });
        int row = i / rowSize;
        int column = i % rowSize;
        return Vec2(positionInterval * column, - positionInterval * row);
    }

    bool HpBar::init()
    {
        if (!Node::init()) return false;

        static int hp = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "hp" });
        static float scale = Config::getInstance().getDoubleValue({ "UI", "HP", "scale" });

        for (int i = 0; i < hp; ++i)
        {
            auto heart = Sprite::create("UI/heart.png");
            heart->setPosition(heartIndex2Position(i));
            heart->setScale(scale);
            addChild(heart);
            _hearts.pushBack(heart);
        }
        return true;
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
                heart->setPosition(heartIndex2Position(i));
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

        static float fontSize = Config::getInstance().getDoubleValue({ "UI", "score", "scoreFontSize" });
        _score = Label::createWithTTF("0", "fonts/Marker Felt.ttf", fontSize);
        static float scoreR = Config::getInstance().getDoubleValue({ "UI", "score", "scoreR" });
        static float scoreG = Config::getInstance().getDoubleValue({ "UI", "score", "scoreG" });
        static float scoreB = Config::getInstance().getDoubleValue({ "UI", "score", "scoreB" });
        _score->setColor(Color3B(scoreR, scoreG, scoreB));
        addChild(_score);
        return true;
    }

    void ScoreDisplayer::changeScore(int score)
    {
        _score->setString(std::to_string(score));
    }
}
