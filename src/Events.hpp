#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <functional>
#include <utility>
#include "entt/entt.hpp"
#include "Components.hpp"

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

	class SourceSystem
	{
	public:
		void Notify(
			entt::registry& registry,
			entt::entity sourceEntity,
			entt::entity selfEntity,
			ecs::SourceComponent& source,
			events::Events event
		);
		bool TryNotify(
			entt::registry& registry,
			entt::entity sourceEntity,
			entt::entity selfEntity,
			events::Events event
		);
		void AddObserver(
			ecs::SourceComponent& source,
			ecs::ObserverComponent* observer
		);
		void RemoveObserver(
			ecs::SourceComponent& source,
			ecs::ObserverComponent* observer
		);
	};


	using Listener = std::function<void(std::string)>;
	class EventQueue
	{
	private:
		std::array<std::pair<std::uint8_t, Listener>, 256> listeners;
		std::array<std::string, 256> queuedEvents;
		std::uint8_t numberOfEventsInQueue;

	public:
		EventQueue();
		std::uint8_t RegisterListener(Listener listener);
		void DeregisterListener(std::uint8_t identifier);

		void EnqueueEvent(std::string event);
		void BroadcastAllEvents();
	};
}


