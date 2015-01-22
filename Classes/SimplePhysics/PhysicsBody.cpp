#include "PhysicsBody.h"
#include "cocos2d.h"
#include "cmath"
#include "iostream"

USING_NS_CC;

#define SIGN(x) ((x) > 0 ? 1 : ((x) == 0 ? 0 : -1))

namespace joker {

    float PhysicsBody::_groundHeight(200);
    float PhysicsBody::_gravity(120.0f);
    float PhysicsBody::_groundResistance(480.0f);
    float PhysicsBody::_defaultSpeed(160.0f);
    float PhysicsBody::_worldWidth(960.0f);

    PhysicsBody::PhysicsBody(float x, float y, float w, float h) :
        _x(x), _y(y), _w(w), _h(h), _vx(0), _vy(0),
        _landCallback(nullptr), _jumpCallback(nullptr) {
        Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    PhysicsBody::~PhysicsBody() {
        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    }

    void PhysicsBody::setGravity(float newGravity) {
        _gravity = newGravity;
    }

    float PhysicsBody::setVelocityX(float vx) {
        return _vx = vx;
    }

    float PhysicsBody::setVelocityY(float vy) {
        return _vy = vy;
    }

    float PhysicsBody::setResistanceX(float r) {
        return _r = r;
    }

    void PhysicsBody::setGroundHeight(float newGroundHeight) {
        _groundHeight = newGroundHeight;
    }

    void PhysicsBody::setWorldWidth(float worldWidth) {
        _worldWidth = worldWidth;
    }

    void PhysicsBody::jump() {
        if (isJumping())
            return;
        _vy = 150;

        if (_jumpCallback)
            _jumpCallback();
    }

    bool PhysicsBody::isJumping() {
        return _y > _groundHeight;
    }

    void PhysicsBody::update(float dt) {
        _x += _vx * dt;
        _y += _vy * dt;

        _vx = SIGN(_vx) * std::max(std::abs(_vx) - _r * dt, 0.0f);

        if (_y > _groundHeight) {
            _vy -= _gravity  * dt;
        }

        if (_x - _w / 2 < 0)
            _x = _w / 2;
        if (_x + _w / 2 > _worldWidth)
            _x = _worldWidth - _w / 2;

        if (_y < _groundHeight) {
            _y = _groundHeight;
            _vy = 0;
            if (_landCallback)
                _landCallback();
        }
    }
}