#ifndef JOKER_HP_SCORE_BAR
#define JOKER_HP_SCORE_BAR

#include "cocos2d.h"

namespace joker
{
    using cocos2d::Node;
    using cocos2d::Vector;
    using cocos2d::Sprite;

    class HpBar : public Node
    {
    public:
        CREATE_FUNC(HpBar);
        void changeHeartNum(int heartNum);
    private:
        bool init() override;
        Vector<Sprite*> _hearts;
    };
}

#endif
