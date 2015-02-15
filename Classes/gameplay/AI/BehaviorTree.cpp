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
    float EnemyFastRunNode::distance = Config::getInstance().getDoubleValue({"EnemyKeepDistance", "FastRunDistance"});

    EnemyFastRunNode::EnemyFastRunNode(BTprecondition && precondition, Role * role)
        : RoleActionNode(std::move(precondition), role)
    {
    }

    void EnemyFastRunNode::onEnter()
    {
        RoleCommand command(RoleAction::FAST_RUN);
        command.add<RoleDirection>("direction", getRole()->getDirection());
        getRole()->executeCommand(command);
    }

    void EnemyFastRunNode::onExit()
    {
        RoleCommand command(RoleAction::STOP);
        command.add("direction", getRole()->getDirection());
        getRole()->executeCommand(command);
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

        auto fastRunRoot = BTNodePtr(new joker::Sequence([](const BTParam & param){ return true; }));
        auto fastRunPred = BTNodePtr(new DoNothing([enemy](const BTParam & param){
            return std::abs(param.playerPosition - enemy->getPosition().x) > EnemyFastRunNode::distance;
        }, enemy));
        auto fastRun = BTNodePtr(new EnemyFastRunNode([enemy](const BTParam & param){ 
            return std::abs(param.playerPosition - enemy->getPosition().x) > EnemyFastRunNode::distance / 1.2f;
        }, enemy));

        fastRunRoot->addChild(std::move(fastRunPred));
        fastRunRoot->addChild(std::move(fastRun));

        auto rushRoot = BTNodePtr(new joker::Sequence([](const BTParam & param){ return param.closest; }));
        auto rushPred = BTNodePtr(new DoNothing([enemy](const BTParam & param){
            return std::abs(param.playerPosition - enemy->getPosition().x) > EnemyFastRunNode::distance / 1.5f;
        }, enemy));
        auto rush = BTNodePtr(new EnemyFastRunNode([enemy](const BTParam & param){
            return std::abs(param.playerPosition - enemy->getPosition().x) > EnemyFastRunNode::distance / 1.8f;
        }, enemy));

        rushRoot->addChild(std::move(rushPred));
        rushRoot->addChild(std::move(rush));

        auto keepDistance = BTNodePtr(new Selector([](const BTParam & param){ return true; }));
        keepDistance->addChild(std::move(rushRoot));
        keepDistance->addChild(std::move(fastRunRoot));

        par->addChild(std::move(faceToPlayer));
        par->addChild(std::move(keepDistance));

        return par;
    }

}
