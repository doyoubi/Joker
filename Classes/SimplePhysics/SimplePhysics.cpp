#include "SimplePhysics.h"

#define SIGN(x) ((x) > 0 ? 1 : ((x) == 0 ? 0 : -1))

namespace joker {

    float SimplePhysics::_groundHeight(0);
    float SimplePhysics::_gravity(1);
    float SimplePhysics::_groundResistance(0.3f);

    SimplePhysics::SimplePhysics(float x, float y, float w, float h) :
        _x(x), _y(y), _w(w), _h(h), _vx(0), _vy(0),
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

    void SimplePhysics::update() {
        /*
        * �����ٶȸ�������
        */
        _x += _vx;
        _y += _vy;
        /*
        * �����û�������ٶȶ����ڵ��棬���ܵ�������������
        */
        if (_r != 0) {
            float new_vx = _vx - SIGN(_vx) * _r;
            if (SIGN(new_vx) != SIGN(_vx))
                _vx = 0;
            else
                _vx = new_vx;
        }
        /*
        * ��������
        */
        if (_y > _groundHeight) {
            _vy -= _gravity;
        }

        /*
        * �䵽����
        */
        if (_y < _groundHeight) {
            _y = _groundHeight;
            _vy = 0;
            if (_landCallback)
                _landCallback();
        }
    }
}