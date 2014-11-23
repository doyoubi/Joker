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

        // �ص�����
        std::function <void(void)> _landCallback; // ����
        std::function <void(void)> _jumpCallback; // ���
    protected:

        static float _gravity;              // ����
        static float _groundHeight;         // ����߶�
        static float _groundResistance;     // ��������

        float _r;       // ����
        float _x;       // x����
        float _y;       // y����
        float _w;       // ���
        float _h;       // �߶�
        float _vx;      // ˮƽ�����ٶ�
        float _vy;      // ��ֱ�����ٶ�

    };
}

#endif