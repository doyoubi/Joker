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

    // Parallel
    BTNodeStatus Parallel::traverse(const BTParam & param)
    {
        BTNodeStatus status = BTNodeStatus::FAILURE;
        for (auto & ptr : _children)
        {
            status = ptr->tick(param);
            if (status == BTNodeStatus::RUNNING)
            {
                status = BTNodeStatus::RUNNING;
            }
            else if (status != BTNodeStatus::RUNNING 
                && status == BTNodeStatus::SUCCESS)
            {
                status = BTNodeStatus::SUCCESS;
            }
        }
        return status;
    }

    // KeepDistance
    KeepDistance::KeepDistance(BTprecondition && precondition, Role * role)
        : RoleActionNode(std::move(precondition), role)
    {
    }

    void KeepDistance::onEnter()
    {
    }

    void KeepDistance::onExit()
    {
    }

    BTNodeStatus KeepDistance::execute(const BTParam & param)
    {
        using std::abs;
        int distance = getRole()->getPosition().x - param.playerPosition;

        if (abs(distance) > rangeFar) getRole()->executeCommand(RoleCommand(RoleAction::IDLE));
        else getRole()->executeCommand(RoleCommand(RoleAction::DEFENCE));

        if (distance < 0 && abs(distance) < rangeNear
            || distance >= 0 && abs(distance) >= rangeFar)
        {
            getRole()->executeCommand(RoleCommand(RoleAction::LEFT_RUN));
        }
        else if (distance < 0 && abs(distance) >= rangeFar
            || distance >= 0 && abs(distance) <= rangeNear)
        {
            getRole()->executeCommand(RoleCommand(RoleAction::RIGHT_RUN));
        }
        return BTNodeStatus::RUNNING;
    }

    // FaceToPlayer
    FaceToPlayer::FaceToPlayer(BTprecondition && precondition, Role * role)
        : RoleActionNode(std::move(precondition), role)
    {
    }

    BTNodeStatus FaceToPlayer::execute(const BTParam & param)
    {
        if (param.playerPosition < getRole()->getPosition().x)
            getRole()->setDirection(RoleDirection::LEFT);
        else
            getRole()->setDirection(RoleDirection::RIGHT);
        return BTNodeStatus::RUNNING;
    }


    // create behavior tree
    BTNodePtr createEnemyTree(Role * enemy)
    {
        CHECKNULL(enemy);
        auto sel = BTNodePtr(new Selector([](const BTParam & param){
            return true;
        }));

        auto par = BTNodePtr(new Parallel([](const BTParam & param){
            return true;
        }));

        auto faceToPlayer = BTNodePtr(new FaceToPlayer([](const BTParam & param){
            return true;
        }, enemy));

        auto keepDistance = BTNodePtr(new KeepDistance([](const BTParam & param){
            return param.closest;
        }, enemy));

        sel->addChild(std::move(keepDistance));
        par->addChild(std::move(faceToPlayer));
        par->addChild(std::move(sel));

        return par;
    }

}
