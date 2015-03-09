#ifndef JOKER_GLOBAL_VALUE
#define JOKER_GLOBAL_VALUE

namespace joker
{
    // this design size is not the same as Director::getInstance()->getOpenGLView()->getDesignResolutionSize()
    // this is the real design resolution
    // while the latter will be modified since we use ResolutionPolicy::FIXED_HEIGHT
    const int DesignSizeWidth = 960;
    const int DesignSizeHeight = 640;
}

#endif
