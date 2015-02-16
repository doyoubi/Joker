#include "BehaviorTree.h"
#include "role/Role.h"
#include "utils/debug.h"
#include "utils/config.h"

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
            onExit();
        }
        _currStatus = BTNodeStatus::INIT;
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
            else if (status == BTNodeStatus::SUCCESS)
            {
                status = BTNodeStatus::SUCCESS;
            }
        }
        return status;
    }

    // Sequence
    BTNodeStatus Sequence::traverse(const BTParam & param)
    {
        BTNodeStatus status = _children[_currNode]->tick(param);
        if (status == BTNodeStatus::SUCCESS)
        {
            ++_currNode;
            if (_currNode == _children.size())
            {
                _currNode = 0;
                return BTNodeStatus::SUCCESS;
            }
        }
        else if (status == BTNodeStatus::FAILURE)
        {
            _currNode = 0;
            return BTNodeStatus::FAILURE;
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

    // EnemyFastRunNode
    KeepDistanceNode::KeepDistanceNode(BTprecondition && precondition, Role * role, RoleAction moveAction)
        : RoleActionNode(std::move(precondition), role), _moveAction(moveAction)
    {
        DEBUGCHECK(moveAction == RoleAction::RUN
            || moveAction == RoleAction::FAST_RUN
            || moveAction == RoleAction::RETREAT,
            "invalid moveAction");
    }

    void KeepDistanceNode::onEnter()
    {
        RoleCommand command(_moveAction);
        command.add<RoleDirection>("direction", getRole()->getDirection());
        getRole()->executeCommand(command);
    }

    void KeepDistanceNode::onExit()
    {
        RoleCommand command(RoleAction::STOP);
        command.add("direction", getRole()->getDirection());
        getRole()->executeCommand(command);
    }


    BTNodePtr createKeepDisNode(Role * role, RoleAction moveAction, BTprecondition pred, float rangeNear, float rangeFar)
    {
        auto root = BTNodePtr(new joker::Sequence(std::move(pred)));
        auto predNode = BTNodePtr(new DoNothing([role, rangeFar](const BTParam & param){
            return std::abs(param.playerPosition - role->getPosition().x) > rangeFar;
        }, role));
        auto runNode = BTNodePtr(new KeepDistanceNode([role, rangeNear](const BTParam & param){
            return std::abs(param.playerPosition - role->getPosition().x) > rangeNear;
        }, role, moveAction));
        root->addChild(std::move(predNode));
        root->addChild(std::move(runNode));
        return root;
    }


    // create behavior tree
    BTNodePtr createEnemyTree(Role * enemy)
    {
        CHECKNULL(enemy);

        auto par = BTNodePtr(new Parallel([](const BTParam & param){
            return true;
        }));

        auto faceToPlayer = BTNodePtr(new FaceToPlayer([](const BTParam & param){
            return true;
        }, enemy));

        static float NotClosestRangeNear = Config::getInstance().getDoubleValue({ "EnemyKeepDistance", "notClosest", "rangeNear" });
        static float NotClosestRangeFar = Config::getInstance().getDoubleValue({ "EnemyKeepDistance", "notClosest", "rangeFar" });
        
        auto notClosest = createKeepDisNode(enemy, RoleAction::FAST_RUN, 
            [](const BTParam & param){ return true; }, NotClosestRangeNear, NotClosestRangeFar);

        static float closestRangeNear = Config::getInstance().getDoubleValue({ "EnemyKeepDistance", "closest", "rangeNear" });
        static float closestRangeFar = Config::getInstance().getDoubleValue({ "EnemyKeepDistance", "closest", "rangeFar" });

        auto closest = createKeepDisNode(enemy, RoleAction::FAST_RUN,
            [](const BTParam & param){ return param.closest; }, closestRangeNear, closestRangeFar);

        auto keepDistance = BTNodePtr(new Selector([](const BTParam & param){ return true; }));
        keepDistance->addChild(std::move(closest));
        keepDistance->addChild(std::move(notClosest));

        par->addChild(std::move(faceToPlayer));
        par->addChild(std::move(keepDistance));

        return par;
    }

}
