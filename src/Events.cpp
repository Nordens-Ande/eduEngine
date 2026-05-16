#include "Events.hpp"

namespace events
{
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
	void EventQueue::EnqueueEvent(const CollisionEvent& event)
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
