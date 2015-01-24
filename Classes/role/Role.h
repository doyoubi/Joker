#ifndef JOKER_ROLE
#define JOKER_ROLE

#include <memory>
#include <string>

#include "cocos2d.h"
#include "cocostudio/CCArmature.h"

#include "state.h"
#include "RoleEnumType.h"
#include "SimplePhysics/PhysicsBody.h"
#include "utils/VarParams.h"


namespace joker
{
    struct RoleCommand : VarParams
    {
        RoleCommand(RoleAction action)
        : roleAction(action) {}

        RoleAction roleAction;
    };

    class Role : public cocos2d::Node
    {
    public:
        const static int PlayerShortAttackScope = 50;
        const static int PlayerLongAttackScope = 300;
        const static int EnemyAttackScope = 300;

        // load animation project to ArmatureDataManager
        // should be called once before using Role::create
        static void loadAnimationSource();
        static Role * create(const std::string & animationName);

        void setCollideCallbak(PhysicsBody::CollideCallback && collideCallback)
        { _simplePhysicsBody.setCollideCallback(std::move(collideCallback)); }

        void executeCommand(const RoleCommand & command);
        RoleDirection getDirection() const;
        void setDirection(RoleDirection direction);

        cocostudio::Armature * getArmature() { return _armature; }
        std::unique_ptr<StateManager> & getStateManager() { return _stateManager; }

        void setPosition(const cocos2d::Vec2 & position) override;
        void setPosition(float x, float y) override;

        PhysicsBody * getPhysicsBody() { return &_simplePhysicsBody; }

        void setSpeed(float normalSpeed, float slowSpeed){ _normalSpeed = normalSpeed; _slowSpeed = slowSpeed; }
        float getNormalSpeed() const { return _normalSpeed; }
        float getSlowSpeed() const { return _slowSpeed; }

    private:
        Role(cocostudio::Armature * armature);

        std::unique_ptr<StateManager> _stateManager;
        cocostudio::Armature * _armature;
        PhysicsBody _simplePhysicsBody;

        float _normalSpeed = 0;
        float _slowSpeed = 0;
    };


}

#endif
