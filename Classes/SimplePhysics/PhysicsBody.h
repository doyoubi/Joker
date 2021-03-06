#ifndef JOKER_PHYSICS_BODY
#define JOKER_PHYSICS_BODY

#include <functional>

namespace joker {

    struct CollideInfo
    {
        float selfPosition;
        float oppositePosition;
    };

    class PhysicsBody {
    public:
        static float JumpInitSpeedY;

        PhysicsBody(float x, float y, float w, float h);
        ~PhysicsBody();

        inline bool isLanded();

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

        float setWidth(float w)              { return _w = w; }
        float setHeight(float h)             { return _h = h; }
        float setX(float x);
        float setY(float y);
        bool  setCollidable(bool collidable) { return _collidable = collidable; }

        float getX() const               { return _x; }
        float getY() const               { return _y; }
        float getVelocityX() const       { return _vx; }
        float getWidth() const           { return _w; }
        float getHeight() const          { return _h; }
        bool  isCollidable() const      { return _collidable; }

        void update(float);

        typedef std::function<void(const CollideInfo & CollideInfo)> CollideCallback;
        void setCollideCallback(CollideCallback && collideCallback);
        void collide(const CollideInfo & CollideInfo);

        typedef std::function<void(void)> JumpCallback;
        typedef std::function<void(void)> LandCallback;
        void setJumpCallback(JumpCallback && jumpCallback);
        void setLandCallback(LandCallback && landCallback);

    private:

        CollideCallback _collideCallback;

        float _r;
        float _x;
        float _y;
        float _w;
        float _h;
        float _vx;
        float _vy;
        bool _collidable;

        LandCallback _landCallback;
        JumpCallback _jumpCallback;
    };
}

#endif