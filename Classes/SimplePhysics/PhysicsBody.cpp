#include <algorithm>
#include <iostream>
#include <cmath>

#include "cocos2d.h"

#include "PhysicsBody.h"
#include "PhysicsWorld.h"
#include "utils/debug.h"
#include "utils/config.h"

USING_NS_CC;

#define SIGN(x) ((x) > 0 ? 1 : ((x) == 0 ? 0 : -1))

namespace joker {

    float PhysicsBody::JumpInitSpeedY = Config::getInstance().getDoubleValue({ "RoleProperty", "player", "jumpSpeedY" });

    PhysicsBody::PhysicsBody(float x, float y, float w, float h) :
        _x(x), _y(y), _w(w), _h(h), _vx(0), _vy(0), _r(0), _collidable(false),
        _landCallback(nullptr), _jumpCallback(nullptr) 
    {
        PhysicsWorld::getInstance()->addPhysicsBody(this);
    }

    PhysicsBody::~PhysicsBody() {
        PhysicsWorld::getInstance()->removePhysicsBody(this);
    }

    void PhysicsBody::setCollideCallback(CollideCallback && collideCallback)
    {
        CHECKNULL(collideCallback);
        _collideCallback = collideCallback;
    }

    void PhysicsBody::setJumpCallback(JumpCallback && jumpCallback)
    {
        CHECKNULL(jumpCallback);
        _jumpCallback = jumpCallback;
    }

    void PhysicsBody::setLandCallback(LandCallback && landCallback)
    {
        CHECKNULL(landCallback);
        _landCallback = landCallback;
    }

    void PhysicsBody::collide(const CollideInfo & CollideInfo)
    { 
        if (_collideCallback)
            _collideCallback(CollideInfo);
    }

    float PhysicsBody::setX(float x)
    {
        x = std::max(x, 0.0f);
        x = std::min(x, PhysicsWorld::getInstance()->getWorldWidth());
        return _x = x;
    }

    float PhysicsBody::setY(float y)
    {
        y = std::max(y, 0.0f);
        return _y = y;
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

    void PhysicsBody::jump() {
        if (!isLanded())
            return;  // avoid double jump
        _vy = JumpInitSpeedY;

        if (_jumpCallback)
            _jumpCallback();
    }

    bool PhysicsBody::isLanded() {
        return _y <= PhysicsWorld::getInstance()->getGroundHeight();
    }

    void PhysicsBody::update(float dt) {
        _x += _vx * dt;
        _y += _vy * dt;

        _vx = SIGN(_vx) * std::max(std::abs(_vx) - _r * dt, 0.0f);

        if (_y > PhysicsWorld::getInstance()->getGroundHeight()) {
            _vy -= PhysicsWorld::getInstance()->getGravity()  * dt;
        }

        if (_x - _w / 2 < 0)
            _x = _w / 2;
        if (_x + _w / 2 > PhysicsWorld::getInstance()->getWorldWidth())
            _x = PhysicsWorld::getInstance()->getWorldWidth() - _w / 2;

        if (_y < PhysicsWorld::getInstance()->getGroundHeight()) {
            _y = PhysicsWorld::getInstance()->getGroundHeight();
            _vy = 0;
            if (_landCallback)
                _landCallback();
        }
    }
}