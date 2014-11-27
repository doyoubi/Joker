#include "SimplePhysics.h"

#define SIGN(x) ((x) > 0 ? 1 : ((x) == 0 ? 0 : -1))

namespace joker {

    float SimplePhysics::_groundHeight(200);
    float SimplePhysics::_gravity(120.0f);
    float SimplePhysics::_groundResistance(200.0f);
    float SimplePhysics::_defaultSpeed(160.0f);

    SimplePhysics::SimplePhysics(float x, float y, float w, float h) :
        _x(x), _y(y), _w(w), _h(h), _vx(0), _vy(0),
        _landCallback(nullptr), _jumpCallback(nullptr) {
        /*CCDirector::sharedDirector()->getScheduler()->
            schedule((SEL_SCHEDULE)(SimplePhysics::update), nullptr, 0.0f, CC_REPEAT_FOREVER, 0.0f, false);
        */
    }

    SimplePhysics::~SimplePhysics() {
        /*CCDirector::sharedDirector()->getScheduler()->
            unschedule((SEL_SCHEDULE)(SimplePhysics::update),nullptr);
            */
    }

    void SimplePhysics::setGravity(float newGravity) {
        _gravity = newGravity;
    }

    float SimplePhysics::setVelocityX(float vx) {
        return _vx = vx;
    }

    float SimplePhysics::setVelocityY(float vy) {
        return _vy = vy;
    }

    float SimplePhysics::setResistanceX(float r) {
        return _r = r;
    }

    void SimplePhysics::setGroundHeight(float newGroundHeight) {
        _groundHeight = newGroundHeight;
    }

    void SimplePhysics::jump() {
        if (isJumping())
            return;
        _vy = 20;

        if (_jumpCallback)
            _jumpCallback();
    }

    bool SimplePhysics::isJumping() {
        return _y > _groundHeight;
    }

    void SimplePhysics::update(float dt) {
        /*
        * 根据速度更新坐标
        */
        _x += _vx * dt;
        _y += _vy * dt;
        /*
        * 如果在没有锁定速度而且在地面，就受到反向阻力作用
        */
        if (_r != 0) {
            float new_vx = _vx - SIGN(_vx) * _r  * dt;
            if (SIGN(new_vx) != SIGN(_vx))
                _vx = 0;
            else
                _vx = new_vx;
        }
        /*
        * 处理重力
        */
        if (_y > _groundHeight) {
            _vy -= _gravity  * dt;
        }

        /*
        * 落到地面
        */
        if (_y < _groundHeight) {
            _y = _groundHeight;
            _vy = 0;
            if (_landCallback)
                _landCallback();
        }
    }
}