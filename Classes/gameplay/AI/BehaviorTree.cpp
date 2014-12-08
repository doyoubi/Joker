#include "BehaviorTree.h"
#include "role/Role.h"
#include "utils/debug.h"

namespace joker
{

    // BTNode
    BTNode::BTNode(BTprecondition && precondition)
        : _precondition(std::move(precondition))
    {
        CHECKNULL(_precondition);
    }

    BTNodeStatus BTNode::tick(const BTParam & param)
    {
        if (!_precondition(param))
        {
            setInitStatus();
            return BTNodeStatus::FAILURE;
        }

        return traverse(param);
    }


    // ControlNode
    void ControlNode::addChild(BTNodePtr && node)
    {
        _children.push_back(std::move(node));
    }

    void ControlNode::setInitStatus()
    {
        for (auto & ptr : _children)
        {
            ptr->setInitStatus();
        }
    }


    // ActionNode
    BTNodeStatus ActionNode::traverse(const BTParam & param)
    {
        if (_currStatus != BTNodeStatus::RUNNING)
            onEnter();
        
        _currStatus = execute(param);

        if (_currStatus != BTNodeStatus::RUNNING)
            onExit();

        return _currStatus;
    }

    void ActionNode::setInitStatus()
    {
        if (_currStatus == BTNodeStatus::RUNNING)
        {
            _currStatus = BTNodeStatus::INIT;
            onExit();
        }
    }

    void ActionNode::addChild(BTNodePtr && node)
    {
        DEBUGCHECK(true, "ActionNode::addChild should not be called");
    }


    // RoleActionNode
    RoleActionNode::RoleActionNode(BTprecondition && precondition, Role * role)
        : ActionNode(std::move(precondition)), _role(role)
    {
        CHECKNULL(_role);
    }


    // Selector
    BTNodeStatus Selector::traverse(const BTParam & param)
    {
        // Keep going until a child says its running.
        for (auto & ptr : _children)
        {
            BTNodeStatus status = ptr->tick(param);
            if (status == BTNodeStatus::RUNNING || status == BTNodeStatus::SUCCESS)
            {
                return status;
            }
        }
        return BTNodeStatus::FAILURE; // return failure when there is not running node
    }

    // KeepDistance
    KeepDistance::KeepDistance(BTprecondition && precondition, Role * role)
        : RoleActionNode(std::move(precondition), role)
    {
    }

    BTNodeStatus KeepDistance::execute(const BTParam & param)
    {
        using std::abs;
        int distance = getRole()->getPosition().x - param.playerPosition;
        if (distance < 0 && abs(distance) < rangeNear
            || distance >= 0 && abs(distance) >= rangeFar)
        {
            getRole()->executeCommand(RoleAction::LEFT_RUN);
        }
        if (distance < 0 && abs(distance) >= rangeFar
            || distance >= 0 && abs(distance) <= rangeNear)
        {
            getRole()->executeCommand(RoleAction::RIGHT_RUN);
        }
        return BTNodeStatus::RUNNING;
    }


    // create behavior tree
    BTNodePtr createEnemyTree(Role * enemy)
    {
        CHECKNULL(enemy);
        auto root = BTNodePtr(new Selector([](const BTParam & param){
            return true;
        }));

        auto keepDistance = BTNodePtr(new KeepDistance([](const BTParam & param){
            return param.closest;
        }, enemy));

        root->addChild(std::move(keepDistance));

        return root;
    }

}
