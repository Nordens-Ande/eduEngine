#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <functional>
#include <utility>
#include "entt/entt.hpp"

//class events
//{
//};

namespace events
{
	enum Events
	{
		EVENT_ATTACKED,
		EVENT_DIED,
		EVENT_COLLISION
	};

	struct CollisionEvent
	{
		entt::registry* registry;
		entt::entity thisEntity;
		entt::entity otherEntity;
	};

	using Listener = std::function<void(const CollisionEvent&)>;
	class EventQueue
	{
	private:
		std::array<std::pair<std::uint8_t, Listener>, 256> listeners;
		std::array<CollisionEvent, 256> queuedEvents;

	public:
		std::uint8_t numberOfEventsInQueue;

		EventQueue();
		std::uint8_t RegisterListener(Listener listener);
		void DeregisterListener(std::uint8_t identifier);

		void EnqueueEvent(const CollisionEvent& event);
		void BroadcastAllEvents();
	};
}


