#pragma once

#include <unordered_map>
#include <functional>
#include <string>

#include "event.hpp"

using EventTrigger = std::string;

class EventBehaviour {
private:
	std::unordered_map<EventTrigger, Event> events{};
public:
	Event register_event(EventTrigger trigger);
	void subscribe(EventTrigger trigger, std::function<void()> event_callback);
};