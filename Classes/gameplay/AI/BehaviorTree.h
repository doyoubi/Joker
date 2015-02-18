#ifndef JOKER_BEHAVIOR_TREE
#define JOKER_BEHAVIOR_TREE

#include <functional>
#include <vector>
#include <memory>

#include "role/RoleEnumType.h"

namespace joker
{
    class BTNode;

    enum class BTEvent
    {
        NO_EVENT,
        READY_TO_ATTACK,
    };

    struct BTParam
    {
        int playerPosition;
        bool closest;
        int playerWidth;
        BTEvent event;
    };

    typedef std::function<bool(const BTParam &)> BTprecondition;
    typedef std::unique_ptr<BTNode> BTNodePtr;

    enum class BTNodeStatus
    {
        RUNNING,
        SUCCESS,
        FAILURE,
        INIT,
    };

    // behavior tree node
    class BTNode
    {
    public:
        BTNode(BTprecondition && precondition);

        BTNodeStatus tick(const BTParam & param);
        virtual void setInitStatus() = 0;
        virtual void addChild(BTNodePtr && node) = 0;

    private:
        virtual BTNodeStatus traverse(const BTParam & param) = 0;

        BTprecondition _precondition;

        BTNode(const BTNode&) = delete;
        void operator=(const BTNode&) = delete;
    };


    class ControlNode : public BTNode
    {
    public:
        ControlNode(BTprecondition && precondition) : BTNode(std::move(precondition)) {};
        void addChild(BTNodePtr && node) override;
        void setInitStatus() override;
    protected:
        std::vector<BTNodePtr> _children;
    };


    class ActionNode : public BTNode
    {
    public:
        ActionNode(BTprecondition && precondition) : BTNode(std::move(precondition)) {};
        virtual void onEnter() {};
        virtual void onExit() {};
        void addChild(BTNodePtr && node) final;
        void setInitStatus() override;
        BTNodeStatus getCurrStatus() { return _currStatus; }
    private:
        BTNodeStatus traverse(const BTParam & param) final;
        virtual BTNodeStatus execute(const BTParam & param) = 0;

        BTNodeStatus _currStatus = BTNodeStatus::INIT;
    };


    class Role;

    class RoleActionNode : public ActionNode
    {
    public:
        RoleActionNode(BTprecondition && precondition, Role * role);
        Role * getRole() { return _role; }
        void setExit(bool exit) { _exit = exit; }
    private:
        Role * _role;
    protected:
        bool _exit = false;
    };


    class Selector : public ControlNode
    {
    public:
        Selector(BTprecondition && precondition) : ControlNode(std::move(precondition)) {};
    private:
        BTNodeStatus traverse(const BTParam & param) override;
    };

    class Parallel : public ControlNode
    {
    public:
        Parallel(BTprecondition && precondition) : ControlNode(std::move(precondition)) {};
    private:
        BTNodeStatus traverse(const BTParam & param) override;
    };

    class Sequence : public ControlNode
    {
    public:
        Sequence(BTprecondition && precondition) : ControlNode(std::move(precondition)) {};
        void setInitStatus() override;
    private:
        BTNodeStatus traverse(const BTParam & param) override;
        int _currNode = 0;
    };

    class DoNothing : public RoleActionNode
    {
    public:
        DoNothing(BTprecondition && precondition, Role * role) : RoleActionNode(std::move(precondition), role) {}
    private:
        BTNodeStatus execute(const BTParam & param) override { return BTNodeStatus::SUCCESS; };
    };

    class FaceToPlayer : public RoleActionNode
    {
    public:
        FaceToPlayer(BTprecondition && precondition, Role * role);
    private:
        BTNodeStatus execute(const BTParam & param) override;
    };

    class KeepDistanceNode : public RoleActionNode
    {
    public:
        KeepDistanceNode(BTprecondition && precondition, Role * role, RoleAction moveAction);
    private:
        virtual void onEnter() override;
        virtual void onExit() override;
        BTNodeStatus execute(const BTParam & param) override;
        RoleAction _moveAction;
    };

    class RetreatNode : public RoleActionNode
    {
    public:
        RetreatNode(BTprecondition && precondition, Role * role) : RoleActionNode(std::move(precondition), role) {}
    private:
        virtual void onEnter() override;
        virtual void onExit() override;
        BTNodeStatus execute(const BTParam & param) override;
    };

    BTNodePtr createEnemyTree(Role * enemy);

}

#endif
