#include "eventsystem.hpp"

Event EventSystem::register_event(EventTrigger trigger) {
    Event event{};
    events[trigger] = event;

    return event;
}