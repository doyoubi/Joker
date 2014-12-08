#ifndef JOKER_BEHAVIOR_TREE
#define JOKER_BEHAVIOR_TREE

#include <functional>
#include <vector>
#include <memory>

namespace joker
{
    class BTNode;

    struct BTParam
    {
        int playerPosition;
        bool closest;
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
    private:
        Role * _role;
    };


    class Selector : public ControlNode
    {
    public:
        Selector(BTprecondition && precondition) : ControlNode(std::move(precondition)) {};
    private:
        BTNodeStatus traverse(const BTParam & param) override;
    };


    class KeepDistance : public RoleActionNode
    {
    public:
        KeepDistance(BTprecondition && precondition, Role * role);
    private:
        BTNodeStatus execute(const BTParam & param) override;

        const int rangeNear = 150;
        const int rangeFar = 200;
    };

    class GetClose : public RoleActionNode
    {
    public:
        GetClose(BTprecondition && precondition, Role * role);
    private:
        BTNodeStatus execute(const BTParam & param) override;
    };

    class AvoidToOtherSide : public RoleActionNode
    {
    public:
        AvoidToOtherSide(BTprecondition && precondition, Role * role);
    private:
        BTNodeStatus execute(const BTParam & param) override;
    };


    BTNodePtr createEnemyTree(Role * enemy);

}

#endif
