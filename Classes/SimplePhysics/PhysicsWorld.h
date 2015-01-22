#ifndef JOKER_PHYSICS_WORLD
#define JOKER_PHYSICS_WORLD

#include <vector>

#include "PhysicsBody.h"

namespace joker
{
    class PhysicsWorld
    {
    public:
        PhysicsWorld();
        ~PhysicsWorld();

        static PhysicsWorld * getInstance();
        void addPhysicsBody(PhysicsBody * physicsBody);
        void removePhysicsBody(PhysicsBody * physicsBody);

        void setGravity(float gravity);
        void setGroundHeight(float groundHeight);
        void setWorldWidth(float worldWidth);
        void setResistance(float resistance);

        float getGravity() const { return _gravity; }
        float getGroundHeight() const { return _groundHeight; }
        float getWorldWidth() const { return _worldWidth; }
        float getResistance() const { return _groundResistance; }

        void update(float dt);

    private:
        
        float _gravity = 0.0f;
        float _groundHeight = 0.0f;
        float _worldWidth = 0.0f;
        float _groundResistance = 0.0f;
        std::vector<PhysicsBody*> _physicsBodyList;
    };
}

#endif
