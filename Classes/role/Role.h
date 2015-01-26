#ifndef JOKER_ROLE
#define JOKER_ROLE

#include <memory>
#include <string>

#include "state.h"
#include "RoleEnumType.h"
#include "SimplePhysics/PhysicsBody.h"
#include "utils/VarParams.h"
#include "scene/RoleSprite.h"


namespace joker
{
    struct RoleCommand : public VarParams
    {
        RoleCommand(RoleAction action)
        : roleAction(action) {}

        RoleAction roleAction;
    };

    class Role;
    typedef std::unique_ptr<Role> RolePtr;

    class Role
    {
    public:
        const static int PlayerShortAttackScope = 50;
        const static int PlayerLongAttackScope = 300;
        const static int EnemyAttackScope = 300;

        Role(RoleSprite * roleSprite);
        ~Role();

        void setCollideCallbak(PhysicsBody::CollideCallback && collideCallback)
        { _simplePhysicsBody.setCollideCallback(std::move(collideCallback)); }

        void executeCommand(const RoleCommand & command);
        RoleDirection getDirection() const;
        void setDirection(RoleDirection direction);

        std::unique_ptr<StateManager> & getStateManager() { return _stateManager; }

        void setPosition(const cocos2d::Vec2 & position);
        void setPosition(float x, float y);
        Vec2 getPosition() const;

        PhysicsBody * getPhysicsBody() { return &_simplePhysicsBody; }

        void setSpeed(float normalSpeed, float slowSpeed){ _normalSpeed = normalSpeed; _slowSpeed = slowSpeed; }
        float getNormalSpeed() const { return _normalSpeed; }
        float getSlowSpeed() const { return _slowSpeed; }

        bool isPlayer() const { return _isPlayer; }
        void setIsPlayer() { _isPlayer = true; }

        cocostudio::Armature * getArmature() { return _roleSprite->getArmature(); }

    private:
        std::unique_ptr<StateManager> _stateManager;
        PhysicsBody _simplePhysicsBody;

        float _normalSpeed = 0;
        float _slowSpeed = 0;

        bool _isPlayer = false;

        RoleSprite * _roleSprite; // weak reference
    };


}

#endif
