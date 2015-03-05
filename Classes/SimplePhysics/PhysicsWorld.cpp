#include <algorithm>

#include "cocos2d.h"

#include "PhysicsWorld.h"
#include "utils/debug.h"

namespace joker
{
    PhysicsWorld * PhysicsWorld::globalPhysicsWorld= nullptr;

    PhysicsWorld * PhysicsWorld::getInstance()
    {
        CHECKNULL(globalPhysicsWorld);
        return globalPhysicsWorld;
    }

    void PhysicsWorld::setGlobalPhysicsWorld(PhysicsWorld * physicsWorld)
    {
        CHECKNULL(physicsWorld);
        globalPhysicsWorld = physicsWorld;
    }

    PhysicsWorld::PhysicsWorld()
    {
        cocos2d::Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    PhysicsWorld::~PhysicsWorld()
    {
        cocos2d::Director::getInstance()->getScheduler()->unscheduleUpdate(this);
        globalPhysicsWorld = nullptr;
    }

    void PhysicsWorld::addPhysicsBody(PhysicsBody * physicsBody)
    {
        using namespace std;
        CHECKNULL(physicsBody);
        DEBUGCHECK(end(_physicsBodyList) == find(begin(_physicsBodyList), end(_physicsBodyList), physicsBody),
            "physics body already exist in physics world");
        _physicsBodyList.push_back(physicsBody);
    }

    void PhysicsWorld::removePhysicsBody(PhysicsBody * physicsBody)
    {
        using namespace std;
        CHECKNULL(physicsBody);
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

        using namespace std;
        auto phyList = _physicsBodyList;
        sort(begin(phyList), end(phyList), [](PhysicsBody * lhs, PhysicsBody * rhs){
            return lhs->getX() < rhs->getX();
        });

        for (int i = 0; i < int(phyList.size()) - 1; i++)
        {
            auto left = phyList[i];
            auto right = phyList[i + 1];
            float distance = right->getX() - left->getX();
            // if enemy is collidable(defence state), then allow collide
            if (!(left->isCollidable() && right->isCollidable()))
                continue;
            if (distance >= (left->getWidth() + right->getWidth()) / 2.0f)
                continue;
            CollideInfo leftInfo{ left->getX(), right->getX() };
            CollideInfo rightInfo{ right->getX(), left->getX() };
            left->collide(leftInfo);
            right->collide(rightInfo);
        }
    }
}
