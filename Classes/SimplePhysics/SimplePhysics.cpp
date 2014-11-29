#include "SimplePhysics.h"
#include "cocos2d.h"
#include "cmath"
#include "iostream"

USING_NS_CC;

#define SIGN(x) ((x) > 0 ? 1 : ((x) == 0 ? 0 : -1))

namespace joker {

    float SimplePhysics::_groundHeight(200);
    float SimplePhysics::_gravity(120.0f);
    float SimplePhysics::_groundResistance(480.0f);
    float SimplePhysics::_defaultSpeed(160.0f);

    SimplePhysics::SimplePhysics(float x, float y, float w, float h) :
        _x(x), _y(y), _w(w), _h(h), _vx(0), _vy(0),
        _landCallback(nullptr), _jumpCallback(nullptr) {
        Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    SimplePhysics::~SimplePhysics() {
        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
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
        _vy = 150;

        if (_jumpCallback)
            _jumpCallback();
    }

    bool SimplePhysics::isJumping() {
        return _y > _groundHeight;
    }

    void SimplePhysics::update(float dt) {
        _x += _vx * dt;
        _y += _vy * dt;

        _vx = SIGN(_vx) * std::max(std::abs(_vx) - _r * dt, 0.0f);

        if (_y > _groundHeight) {
            _vy -= _gravity  * dt;
        }

        if (_y < _groundHeight) {
            _y = _groundHeight;
            _vy = 0;
            if (_landCallback)
                _landCallback();
        }
    }
}