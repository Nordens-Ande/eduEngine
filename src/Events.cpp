#include "Events.hpp"

namespace events
{
    void SourceSystem::Notify(
        entt::registry& registry,
        entt::entity sourceEntity,
        entt::entity selfEntity,
        ecs::SourceComponent& source,
        events::Events event)
    {
        for (int i = 0; i < source.numberOfObservers; i++)
        {
            //std::cout << "Notifying" << source.numberOfObservers << std::endl;
            source.observers[i]->OnNotify(registry, sourceEntity, selfEntity, event);
        }
    };
    bool SourceSystem::TryNotify(
        entt::registry& registry,
        entt::entity sourceEntity,
        entt::entity selfEntity,
        events::Events event)
    {
        if (auto* source = registry.try_get<ecs::SourceComponent>(sourceEntity))
        {
            this->Notify(registry, sourceEntity, selfEntity, *source, event);
            return true;
        }
        return false;
    };
    void SourceSystem::AddObserver(
        ecs::SourceComponent& source,
        ecs::ObserverComponent* observer)
    {
        source.observers[source.numberOfObservers] = observer;
        source.numberOfObservers++;
    }
    void SourceSystem::RemoveObserver(
        ecs::SourceComponent& source,
        ecs::ObserverComponent* observer)
    {
        int index = -1;
        for (int i = 0; i < source.numberOfObservers; ++i)
        {
            if (source.observers[i] != observer)
                continue;
            index = i;
            break;
        }

        if (index == -1)
            return;

        for (int i = index; i < source.numberOfObservers - 1; ++i)
        {
            source.observers[i] = source.observers[i + 1];
        }
        source.numberOfObservers--;
    }


	EventQueue::EventQueue()
	{
		numberOfEventsInQueue = 0;
		int id = 0;
		for (auto& pair : listeners)
			pair.first = id++;
	}
	std::uint8_t EventQueue::RegisterListener(Listener listener)
	{
		for (auto& pair : listeners)
		{
			if (!pair.second)
			{
				pair.second = listener;
				return pair.first;
			}
		}
		return 255;
	}
	void EventQueue::DeregisterListener(std::uint8_t idenitifer)
	{
		listeners[idenitifer].second = nullptr;
	}
	void EventQueue::EnqueueEvent(std::string event)
	{
		if (numberOfEventsInQueue == 255) return;
		queuedEvents[numberOfEventsInQueue++] = event;
	}
	void EventQueue::BroadcastAllEvents()
	{
		for (int i = 0; i < numberOfEventsInQueue; i++)
		{
			for (auto& pair : listeners)
				pair.second(queuedEvents[i]);
		}
		numberOfEventsInQueue = 0;
	}
}
