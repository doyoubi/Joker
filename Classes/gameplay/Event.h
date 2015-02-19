#ifndef JOKER_EVENT
#define JOKER_EVENT

#include <unordered_map>
#include <memory>

#include "role/Role.h"

namespace joker
{
    class DirectorEvent;
    class BattleDirector;
    typedef std::unique_ptr<DirectorEvent> EventPtr;

    // This event manager is mainly used for postpone function call
    // from event happen time to the beginning of Director::update()

    class DirectorEventManager
    {
    public:
        DirectorEventManager();
        void addEvent(EventPtr && event);
        void executeEvent(BattleDirector * director);
        bool hasEvent();
    private:
        std::vector<EventPtr> _eventPool;
        bool _addEventLock = false;
    };


    class DirectorEvent
    {
    public:
        virtual void execute(BattleDirector * director) = 0;
    };

    class PlayerAttackEvent : public DirectorEvent
    {
    public:
        virtual void execute(BattleDirector * director) override;
    };

    class EnemyAttackEvent : public DirectorEvent
    {
    public:
        virtual void execute(BattleDirector * director) override;
    };

    class NodEvent : public DirectorEvent
    {
    public:
        virtual void execute(BattleDirector * director) override;
    };

    class CollideEvent : public DirectorEvent
    {
    public:
        CollideEvent(RoleDirection direction)
            : _direction(direction) {}
        virtual void execute(BattleDirector * director) override;
    private:
        RoleDirection _direction;
    };

    class EmptyAttackEvent : public DirectorEvent
    {
    public:
        void execute(BattleDirector * director) override;
    };

    class RemoveRoleEvent : public DirectorEvent
    {
    public:
        RemoveRoleEvent(Role * role) : _role(role) {}
        void execute(BattleDirector * director) override;
    private:
        Role * _role;
    };

    class SpikeAttackEvent : public DirectorEvent
    {
    public:
        void execute(BattleDirector * director) override;
    };

    class AttackedBySpikeEvent : public DirectorEvent
    {
    public:
        void execute(BattleDirector * director) override;
    };


}

#endif
