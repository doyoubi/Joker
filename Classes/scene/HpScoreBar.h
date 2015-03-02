#ifndef JOKER_HP_SCORE_BAR
#define JOKER_HP_SCORE_BAR

#include "cocos2d.h"

namespace joker
{
    using cocos2d::Node;
    using cocos2d::Vector;
    using cocos2d::Sprite;
    using cocos2d::Label;

    class HpBar : public Node
    {
    public:
        CREATE_FUNC(HpBar);
        void changeHeartNum(int heartNum);
    private:
        bool init() override;
        Vector<Sprite*> _hearts;
    };

    class ScoreDisplayer : public Node
    {
    public:
        CREATE_FUNC(ScoreDisplayer);
        void changeScore(int score);
    private:
        bool init() override;
        Label * _score; // weak reference
    };
}

#endif
