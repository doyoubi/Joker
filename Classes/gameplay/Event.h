#ifndef JOKER_EVENT
#define JOKER_EVENT

#include <unordered_map>
#include <memory>

namespace joker
{

    enum class DirectorEventType
    {
        ATTACK,
        ATTACKED,
        NOD,
    };

    class DirectorEvent;
    class BattleDirector;
    typedef std::unique_ptr<DirectorEvent> EventPtr;

    class DirectorEventManager
    {
    public:
        DirectorEventManager();
        void activateEvent(DirectorEventType event);
        void executeEvent(BattleDirector * director);
        bool hasEvent();
        bool isActive(DirectorEventType event);
    private:
        std::unordered_map<DirectorEventType, EventPtr> _eventPool;
    };


    class DirectorEvent
    {
    public:
        virtual void execute(BattleDirector * director) = 0;
        bool isActive() { return _active; }
        void activate() { _active = true; }
        void deactivate() { _active = false; }

    private:
        bool _active = false;
    };

    class AttackEvent : public DirectorEvent
    {
    public:
        virtual void execute(BattleDirector * director) override;
    };

    class AttackedEvent : public DirectorEvent
    {
    public:
        virtual void execute(BattleDirector * director) override;
    };

    class NodEvent : public DirectorEvent
    {
    public:
        virtual void execute(BattleDirector * director) override;
    };

}

#endif
