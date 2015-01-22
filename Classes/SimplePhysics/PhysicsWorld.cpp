#include <algorithm>

#include "cocos2d.h"

#include "PhysicsWorld.h"
#include "utils/debug.h"

namespace joker
{
    PhysicsWorld * PhysicsWorld::getInstance()
    {
        static PhysicsWorld physicsWorld;
        return &physicsWorld;
    }

    PhysicsWorld::PhysicsWorld()
    {
        cocos2d::Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    PhysicsWorld::~PhysicsWorld()
    {
        cocos2d::Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    }

    void PhysicsWorld::addPhysicsBody(PhysicsBody * physicsBody)
    {
        using namespace std;
        DEBUGCHECK(end(_physicsBodyList) == find(begin(_physicsBodyList), end(_physicsBodyList), physicsBody),
            "physics body already exist in physics world");
        _physicsBodyList.push_back(physicsBody);
    }

    void PhysicsWorld::removePhysicsBody(PhysicsBody * physicsBody)
    {
        using namespace std;
        auto iter = find(begin(_physicsBodyList), end(_physicsBodyList), physicsBody);
        DEBUGCHECK(iter != end(_physicsBodyList), "physics body not exist");
        _physicsBodyList.erase(iter);
    }

    void PhysicsWorld::setGravity(float gravity)
    {
        DEBUGCHECK(gravity >= 0.0f, "invalid gravity");
        _gravity = gravity;
    }

    void PhysicsWorld::setGroundHeight(float groundHeight)
    {
        DEBUGCHECK(groundHeight >= 0.0f, "invalid groundHeight");
        _groundHeight = groundHeight;
    }

    void PhysicsWorld::setWorldWidth(float worldWidth)
    {
        DEBUGCHECK(worldWidth > 0, "invalid worldWidth");
        _worldWidth = worldWidth;
    }

    void PhysicsWorld::setResistance(float resistance)
    {
        DEBUGCHECK(resistance >= 0.0f, "invalid resistance");
        _groundResistance = resistance;
    }


    void PhysicsWorld::update(float dt)
    {
        for (auto * body : _physicsBodyList)
        {
            body->update(dt);
        }
    }
}
