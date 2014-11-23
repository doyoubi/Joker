#include "SimplePhysics.h"

#define SIGN(x) ((x) > 0 ? 1 : ((x) == 0 ? 0 : -1))

namespace joker {

    float SimplePhysics::_groundHeight(0);
    float SimplePhysics::_gravity(1);
    float SimplePhysics::_groundResistance(0.3f);

    SimplePhysics::SimplePhysics(float x, float y, float w, float h) :
        _x(x), _y(y), _w(w), _h(h),_vx(0), _vy(0),
        _landCallback(nullptr), _jumpCallback(nullptr) {
    }

    SimplePhysics * SimplePhysics::create(float x, float y, float w, float h) {
        SimplePhysics * phy = new SimplePhysics(x, y, w, h);
        return phy;
    }

    SimplePhysics * SimplePhysics::create() {
        SimplePhysics * phy = new SimplePhysics(0, 0, 0, 0);
        return phy;
    }

    void SimplePhysics::setGravity(float newGravity) {
        _gravity = newGravity;
    }

    void SimplePhysics::run(float v) {
        _r = 0;
        _vx = v;
    }

    void SimplePhysics::stop() {
        _r = _groundResistance;
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

    void SimplePhysics::update() {
        /*
         * 根据速度更新坐标
         */
        _x += _vx;
        _y += _vy;
        /*
         * 如果在没有锁定速度而且在地面，就受到反向阻力作用
         */
        if (_r != 0) {
            float new_vx = _vx - SIGN(_vx) * _r;
            if (SIGN(new_vx) != SIGN(_vx))
                _vx = 0;
            else
                _vx = new_vx;
        }
        /*
         * 处理重力
         */
        if (_y > _groundHeight) {
            _vy -= _gravity;
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