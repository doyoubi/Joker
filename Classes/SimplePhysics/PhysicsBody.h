#ifndef JOKER_PHYSICS_BODY
#define JOKER_PHYSICS_BODY

#include <functional>

namespace joker {

    class PhysicsBody {
    public:
        PhysicsBody(float x, float y, float w, float h);
        ~PhysicsBody();

        inline bool isJumping();

        void jump();
        /*
        * [example]
        * onKeyDown(let the role began to run)
        *
        *      physics->setVelocityX(vx);
        *      physics->setResistanceX(0);
        *
        * onKeyUp (let the role brake)
        *
        *       physics->setResistanceX(Simple::getResistance());
        *
        * onKeyDown (ley the role jump)
        *
        *       physics->jump();
        */
        float setVelocityX(float vx);
        float setVelocityY(float vy);
        float setResistanceX(float r);

        float setWidth(float w)     { return _w = w; }
        float setHeight(float h)    { return _h = h; }
        float setX(float x)         { return _x = x; }
        float setY(float y)         { return _y = y; }

        float getX()                { return _x; }
        float getY()                { return _y; }
        float getVelocityX()        { return _vx; }
        float getWidth()            { return _w; }
        float getHeight()           { return _h; }

        static float getDefaultSpeed() { return _defaultSpeed; }

        void update(float);

        // callback functions
        std::function <void(void)> _landCallback;
        std::function <void(void)> _jumpCallback;

    private:

        static float _defaultSpeed;

        float _r;
        float _x;
        float _y;
        float _w;
        float _h;
        float _vx;
        float _vy;

    };
}

#endif