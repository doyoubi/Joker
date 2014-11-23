#ifndef _SIMPLE_PHYSICS_H_
#define _SIMPLE_PHYSICS_H_

#include <functional>

namespace joker {

    class SimplePhysics {
    public:
        static SimplePhysics * create(float x, float y, float w, float h);
        static SimplePhysics * create();

        SimplePhysics(float x, float y, float w, float h);

        static void setGravity(float newGravity);
        static void setGroundHeight(float newGroundHeight);

        inline bool isJumping();
        
        void jump();

        void run(float v);
        void stop();
        
        float setWidth(float w)     { return _w = w; }
        float setHeight(float h)    { return _h = h; }
        float setX(float x)         { return _x = x; }
        float setY(float y)         { return _y = y; }

        float getX()                { return _x; }
        float getY()                { return _y; }
        float getWidth()            { return _w; }
        float getHeight()           { return _h; }
        
        void update();

        // 回调函数
        std::function <void(void)> _landCallback; // 起跳
        std::function <void(void)> _jumpCallback; // 落地
    protected:

        static float _gravity;              // 重力
        static float _groundHeight;         // 地面高度
        static float _groundResistance;     // 地面阻力

        float _r;       // 阻力
        float _x;       // x坐标
        float _y;       // y坐标
        float _w;       // 宽度
        float _h;       // 高度
        float _vx;      // 水平方向速度
        float _vy;      // 垂直方向速度

    };
}

#endif