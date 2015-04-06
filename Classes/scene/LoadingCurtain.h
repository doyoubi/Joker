#ifndef JOKER_LOADING_CURTAIN
#define JOKER_LOADING_CURTAIN

#include <functional>
#include <string>

#include "cocos2d.h"

#include "utils/AnimationSprite.h"

namespace joker
{
    class LoadingCurtain : public cocos2d::Node
    {
    public:
        CREATE_FUNC(LoadingCurtain);
        void fallDown();
        void drawUp();
        void loading();
        typedef std::function<void(void)> DrawUpEndCallback;
        void setDrawUpEndCallback(DrawUpEndCallback callback);
    private:
        bool init() override;
        AnimationSprite * _loadingCurtain;
    };
}

#endif
