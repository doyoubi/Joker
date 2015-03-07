#ifndef JOKER_LOADING_CURTAIN
#define JOKER_LOADING_CURTAIN

#include <functional>

#include "cocos2d.h"

namespace joker
{
    class LoadingCurtain : public cocos2d::Node
    {
    public:
        CREATE_FUNC(LoadingCurtain);
        void fallDown();
        void drawUp();
        void loading();
        typedef std::function<void(
            cocostudio::Armature *armature,
            cocostudio::MovementEventType movementType,
            const std::string& movementID)> DrawUpEndCallback;
        void setDrawUpEndCallback(DrawUpEndCallback callback);
    private:
        bool init() override;
        cocostudio::Armature * _loadingCurtain;
    };
}

#endif
