#include "debug.h"
#include <fstream>

namespace joker
{
    using std::endl;
    using std::cout;
    using std::cerr;
    using std::string;
    using namespace cocos2d;

    std::ostream & operator << (std::ostream & out, const Vec2 & v)
    {
        out << v.x << ' ' << v.y << endl;
        return out;
    }

    void checkGLError(const char * file, int line)
    {
        GLenum err(glGetError());
        if (err == GL_NO_ERROR) return;
        string error;
        switch (err)
        {
        case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
        case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
        case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
        case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
        default:                        error = "invalid error message";  break;
        }
        cerr << "GL_" << error.c_str() << " - " << file << ":" << line << endl;
        exit(1);
    }


    void toDebugFile(const char * msg)
    {
        static bool firstOpen = true;
        std::ofstream file;
        if (firstOpen) file.open("debugFile.txt");
        else file.open("debugFile.txt", std::ios::app);
        firstOpen = false;
        file << msg << endl;
        cout << msg << endl;
        file.close();
    }

    bool checkVectorNormalized(const Vec3 & v)
    {
        return abs(v.length() - 1) < errorThreshold;
    }

    void executeOnce(std::function<void(void)> func)
    {
        static bool completed = false;
        if (completed) return;
        func();
        completed = true;
    }

    void echoVec2(const Vec2 & v)
    {
        cout << v.x << ' ' << v.y << endl;
    }

}