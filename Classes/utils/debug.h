#ifndef JOKER_DEBUG
#define JOKER_DEBUG

#include <iostream>
#include <string>
#include <functional>
#include "cocos2d.h"
#include <fstream>

#include "HelloWorldScene.h"

namespace joker
{
    using std::endl;
    using std::cout;
    using std::cerr;
    using std::string;
    using namespace cocos2d;

    const float errorThreshold = 0.00000001f;

    std::ostream & operator << (std::ostream & out, const Vec2 & v);

    void checkGLError(const char * file, int line);

    template<class MSG>
    void debugCheck(bool checkedExpression,
        const char * filename,
        int line,
        MSG errorMsg)
    {
        if (checkedExpression) return;
        std::cerr << filename << " : " << line << endl
            << errorMsg << endl;
        std::ofstream file("debugOutput.txt");
        file << filename << " : " << line << endl
            << errorMsg << endl;
        file.close();
        exit(1);
    }

    inline void debugCheckInLogcat(bool checkedExpression,
        const char * filename,
        int line,
        std::string errorMsg)
    {
        if (checkedExpression) return;
        CCLOGERROR("cocosdyb: %s", errorMsg.c_str());
    }

    inline void printCodeLocation(const char * filename, int line)
    {
        CCLOG("cocosdyb:  %s: line %d", filename, line);
    }

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#define DEBUGCHECK(checkedExpression, errMsg) \
    debugCheck(checkedExpression, __FILE__, __LINE__, errMsg)

#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#define DEBUGCHECK(checkedExpression, errMsg) \
    debugCheckInLogcat(checkedExpression, __FILE__, __LINE__, errMsg)

#endif


#define codeLoc() \
    printCodeLocation(__FILE__, __LINE__)

#define CHECKNULL(ptr) DEBUGCHECK(ptr != nullptr, "null pointer")

#define ERRORMSG(errorMsg) DEBUGCHECK(false, errorMsg) 

    void toDebugFile(const char * msg);

    bool checkVectorNormalized(const Vec3 & v);

    void echoVec2(const Vec2 & v);

    void executeOnce(std::function<void(void)> func);
}

#endif